#include "GraphicsCore.h"
#include "CommandListManager.h"

namespace re::engine::render {

static const uint32_t vendorID_Nvidia = 0x10DE;
static const uint32_t vendorID_AMD = 0x1002;
static const uint32_t vendorID_Intel = 0x8086;

const wchar_t* GPUVendorToString(uint32_t vendorID) {
  switch (vendorID) {
    case vendorID_Nvidia:
      return L"Nvidia";
    case vendorID_AMD:
      return L"AMD";
    case vendorID_Intel:
      return L"Intel";
    default:
      return L"Unknown";
      break;
  }
}

uint32_t GetVendorIdFromDevice(ID3D12Device* pDevice) {
  LUID luid = pDevice->GetAdapterLuid();

  // Obtain the DXGI factory
  Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
  RE_ASSERT_SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)));

  Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;

  RE_ASSERT_SUCCEEDED(dxgiFactory->EnumAdapterByLuid(luid, IID_PPV_ARGS(&pAdapter)));
  DXGI_ADAPTER_DESC1 desc;
  RE_ASSERT_SUCCEEDED(pAdapter->GetDesc1(&desc));

  return desc.VendorId;
}

bool GraphicsCore::IsDeviceNvidia() {
  return GetVendorIdFromDevice(m_Device.Get()) == vendorID_Nvidia;
}
bool GraphicsCore::IsDeviceAMD() {
  return GetVendorIdFromDevice(m_Device.Get()) == vendorID_AMD;
}
bool GraphicsCore::IsDeviceIntel() {
  return GetVendorIdFromDevice(m_Device.Get()) == vendorID_Intel;
}

GraphicsCore::GraphicsCore() {}
GraphicsCore::~GraphicsCore() {
  Release();
}

bool IsDirectXRaytracingSupported(ID3D12Device* testDevice) {
  D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupport = {};

  if (FAILED(testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupport, sizeof(featureSupport))))
    return false;

  return featureSupport.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}

void GraphicsCore::Initialize(GCInitInfo info) {
  Microsoft::WRL::ComPtr<ID3D12Device> pDevice;

  DWORD dxgiFactoryFlags = 0;

  if (info.enableDebugLayer) {
    Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))) {
      debugInterface->EnableDebugLayer();

      if (info.enableGPUBasedValidation) {
        Microsoft::WRL::ComPtr<ID3D12Debug1> debugInterface1;
        if (SUCCEEDED((debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1))))) {
          debugInterface1->SetEnableGPUBasedValidation(true);
        }
      }
    } else {
      RE_WARN_ONCE_IF(0, "WARNING:  Unable to enable D3D12 debug validation layer\n")
    }

    if (info.enableDXGIDebugInfo) {
      ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
      if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf())))) {
        dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
        dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

        DXGI_INFO_QUEUE_MESSAGE_ID hide[] = {
            80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */
            ,
        };
        DXGI_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = _countof(hide);
        filter.DenyList.pIDList = hide;
        dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
      }
    }
  }

  // Obtain the DXGI factory
  Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
  RE_ASSERT_SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

  // Create the D3D graphics device
  Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;

  // Temporary workaround because SetStablePowerState() is crashing
  D3D12EnableExperimentalFeatures(0, nullptr, nullptr, nullptr);

  if (!info.useWarpDriver) {
    SIZE_T MaxSize = 0;

    Microsoft::WRL::ComPtr<ID3D12Device> pDevice;
    for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(Idx, &pAdapter); ++Idx) {
      DXGI_ADAPTER_DESC1 desc;
      pAdapter->GetDesc1(&desc);

      // Is a software adapter?
      if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        continue;

      // Can create a D3D12 device?
      if (FAILED(D3D12CreateDevice(pAdapter.Get(), info.d3DFeatureLevel, IID_PPV_ARGS(&pDevice))))
        continue;

      // Does support DXR if required?
      if (info.requireDXRSupport && !IsDirectXRaytracingSupported(pDevice.Get()))
        continue;

      // By default, search for the adapter with the most memory because that's usually the dGPU.
      if (desc.DedicatedVideoMemory < MaxSize)
        continue;

      MaxSize = desc.DedicatedVideoMemory;

      if (m_Device != nullptr) {
        m_Device->Release();
      }

      m_Device = pDevice.Detach();
    }
  } else {
    RE_ASSERT_SUCCEEDED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pAdapter)));
    RE_ASSERT_SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), info.d3DFeatureLevel, IID_PPV_ARGS(&m_Device)));
  }

  if (info.requireDXRSupport && !IsDirectXRaytracingSupported(m_Device.Get())) {
    RE_ASSERT(0, "Unable to find a DXR-capable device. Halting.\n");
  }

  if (m_Device == nullptr) {
    RE_ASSERT(0, "Create device error\n");
  } else if (info.setStablePowerState) {
    bool DeveloperModeEnabled = false;

    // Look in the Windows Registry to determine if Developer Mode is enabled
    HKEY hKey;
    LSTATUS result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock", 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS) {
      DWORD keyValue, keySize = sizeof(DWORD);
      result = RegQueryValueExW(hKey, L"AllowDevelopmentWithoutDevLicense", 0, NULL, (byte*)&keyValue, &keySize);
      if (result == ERROR_SUCCESS && keyValue == 1)
        DeveloperModeEnabled = true;
      RegCloseKey(hKey);
    }

    RE_WARN_ONCE_IF_NOT(DeveloperModeEnabled, "Enable Developer Mode on Windows 10 to get consistent profiling results")

    // Prevent the GPU from overclocking or underclocking to get consistent timings
    if (DeveloperModeEnabled) {
      m_Device->SetStablePowerState(TRUE);
    }
  }

  m_D3DFeatureLevel = info.d3DFeatureLevel;

  if (info.enableDebugLayer) {
    ID3D12InfoQueue* pInfoQueue = nullptr;
    if (SUCCEEDED(m_Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue)))) {
      // Suppress whole categories of messages
      //D3D12_MESSAGE_CATEGORY Categories[] = {};

      // Suppress messages based on their severity level
      D3D12_MESSAGE_SEVERITY Severities[] = {D3D12_MESSAGE_SEVERITY_INFO};

      // Suppress individual messages by their ID
      D3D12_MESSAGE_ID DenyIds[] = {
          // This occurs when there are uninitialized descriptors in a descriptor table, even when a
          // shader does not access the missing descriptors.  I find this is common when switching
          // shader permutations and not wanting to change much code to reorder resources.
          D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

          // Triggered when a shader does not export all color components of a render target, such as
          // when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
          D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

          // This occurs when a descriptor table is unbound even when a shader does not access the missing
          // descriptors.  This is common with a root signature shared between disparate shaders that
          // don't all need the same types of resources.
          D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

          // RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
          D3D12_MESSAGE_ID_RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS,

          // Suppress errors from calling ResolveQueryData with timestamps that weren't requested on a given frame.
          D3D12_MESSAGE_ID_RESOLVE_QUERY_INVALID_QUERY_STATE,

          // Ignoring InitialState D3D12_RESOURCE_STATE_COPY_DEST. Buffers are effectively created in state D3D12_RESOURCE_STATE_COMMON.
          D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED,
      };

      D3D12_INFO_QUEUE_FILTER NewFilter = {};
      //NewFilter.DenyList.NumCategories = _countof(Categories);
      //NewFilter.DenyList.pCategoryList = Categories;
      NewFilter.DenyList.NumSeverities = _countof(Severities);
      NewFilter.DenyList.pSeverityList = Severities;
      NewFilter.DenyList.NumIDs = _countof(DenyIds);
      NewFilter.DenyList.pIDList = DenyIds;

      pInfoQueue->PushStorageFilter(&NewFilter);
      pInfoQueue->Release();
    }
  }

  // We like to do read-modify-write operations on UAVs during post processing.  To support that, we
  // need to either have the hardware do typed UAV loads of R11G11B10_FLOAT or we need to manually
  // decode an R32_UINT representation of the same buffer.  This code determines if we get the hardware
  // load support.
  D3D12_FEATURE_DATA_D3D12_OPTIONS FeatureData = {};
  if (SUCCEEDED(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &FeatureData, sizeof(FeatureData)))) {
    if (FeatureData.TypedUAVLoadAdditionalFormats) {
      D3D12_FEATURE_DATA_FORMAT_SUPPORT Support = {DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE};

      if (SUCCEEDED(m_Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
          (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0) {
        m_bTypedUAVLoadSupport_R11G11B10_FLOAT = true;
      }

      Support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

      if (SUCCEEDED(m_Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
          (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0) {
        m_bTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
      }
    }
  }

  // Command
  m_CommandListManager = GAlloc::make_unique<CommandListManager>();
  m_CommandListManager->SetGraphicsCore(this);
  m_CommandListManager->Initialize();

  // Create swap chain
  DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
  swapChainDesc.BufferCount = info.swapChainBufferCount;
  swapChainDesc.Width = info.swapChainWidth;
  swapChainDesc.Height = info.swapChainHeight;
  swapChainDesc.Format = info.swapChainFormat;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.SwapEffect = info.swapChainEffect;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.Scaling = info.swapChainScaling;
  swapChainDesc.Flags = info.swapChainFlags;

  ComPtr<IDXGISwapChain1> swapChain;
  RE_ASSERT_SUCCEEDED(dxgiFactory->CreateSwapChainForHwnd(m_CommandListManager->GetGraphicsQueue().GetCommandQueue(), info.swapChainHWND,
                                                          &swapChainDesc, nullptr, nullptr, &swapChain));

  // initlize m_FrameIndex
  RE_ASSERT_SUCCEEDED(swapChain.As(&m_SwapChain));
  m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

  // Create descriptor heaps.
  {
    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = info.swapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    RE_ASSERT_SUCCEEDED(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap)));

    m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  }

  // Create frame resources.
  {
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());

    // Create a RTV and a command allocator for each frame.
    m_RenderTargets.resize(info.swapChainBufferCount);
    for (UINT n = 0; n < info.swapChainBufferCount; n++) {
      RE_ASSERT_SUCCEEDED(m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_RenderTargets[n])));
      m_Device->CreateRenderTargetView(m_RenderTargets[n].Get(), nullptr, rtvHandle);
      rtvHandle.Offset(1, m_RtvDescriptorSize);
    }
  }

  //   g_CommandManager.Create(g_Device);

  //   // Common state was moved to GraphicsCommon.*
  //   InitializeCommonState();

  //   Display::Initialize();

  //   GpuTimeManager::Initialize(4096);
  //   TemporalEffects::Initialize();
  //   PostEffects::Initialize();
  //   SSAO::Initialize();
  //   TextRenderer::Initialize();
  //   GraphRenderer::Initialize();
  //   ParticleEffectManager::Initialize(3840, 2160);
}
void GraphicsCore::Release() {
  // m_CommandListManager->IdleGPU();
  // m_ContextManager->Shutdown();

  //   g_CommandManager.IdleGPU();

  //   CommandContext::DestroyAllContexts();
  //   g_CommandManager.Shutdown();
  //   GpuTimeManager::Shutdown();
  //   PSO::DestroyAll();
  //   RootSignature::DestroyAll();
  //   DescriptorAllocator::DestroyAll();

  //   DestroyCommonState();
  //   DestroyRenderingBuffers();
  //   TemporalEffects::Shutdown();
  //   PostEffects::Shutdown();
  //   SSAO::Shutdown();
  //   TextRenderer::Shutdown();
  //   GraphRenderer::Shutdown();
  //   ParticleEffectManager::Shutdown();
  //   Display::Shutdown();

#if defined(_GAMING_DESKTOP) && defined(_DEBUG)
  ID3D12DebugDevice* debugInterface;
  if (SUCCEEDED(g_Device->QueryInterface(&debugInterface))) {
    debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
    debugInterface->Release();
  }
#endif

  if (m_Device != nullptr) {
    m_Device->Release();
    m_Device = nullptr;
  }
}

wstring GraphicsCore::GetAssetFullPath(const wstring& assetName) {
  return m_AssetsPath + assetName;
}

void GraphicsCore::Begin() {
  if (m_CommandList == nullptr) {
    m_CommandListManager->CreateNewCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, &m_CommandList, &m_CommandAllocator);
    for (size_t i = 0; i < 10; i++) {
      m_FrameFenceValues[i] = 0;
    }
  }

  RE_ASSERT_SUCCEEDED(m_CommandList->Reset(m_CommandAllocator, nullptr));

  m_CommandList->RSSetViewports(1, &m_Viewport);
  m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

  // Indicate that the back buffer will be used as a render target.
  auto transition =
      CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
  m_CommandList->ResourceBarrier(1, &transition);

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
  m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

  const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
  m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}
void GraphicsCore::End() {
  // 1. 状态切换回 Present
  auto transition =
      CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
  m_CommandList->ResourceBarrier(1, &transition);

  // 2. 关闭并提交
  RE_ASSERT_SUCCEEDED(m_CommandList->Close());
  // 执行并返回当前任务的 Fence Value
  uint64_t currentFenceValue = m_CommandListManager->GetGraphicsQueue().ExecuteCommandList(m_CommandList);

  // 3. 呈现
  RE_ASSERT_SUCCEEDED(m_SwapChain->Present(1, 0));

  // 4. 更新帧索引（准备下一帧）
  m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

  // 5. 【核心改进】等待下一帧所需的资源释放，而不是等待当前帧完成
  // 假设你有一个数组存储了每一帧最后提交的 FenceValue
  m_CommandListManager->WaitForFence(m_FrameFenceValues[m_FrameIndex]);
  m_FrameFenceValues[m_FrameIndex] = currentFenceValue;

  // 6. 重置状态，以便 Begin 获取
  m_CommandList = nullptr;
}

}  // namespace re::engine::render