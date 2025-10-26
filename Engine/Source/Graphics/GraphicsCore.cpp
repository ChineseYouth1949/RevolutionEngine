#include "GraphicsCore.h"

#include "Base/Result.h"

#include <limits>

namespace RE::Engine {

GraphicsCore::GraphicsCore() {}
GraphicsCore::~GraphicsCore() {}

Result GraphicsCore::Initialize(const GraphicsCoreConfig& config) {
  Result lRes;

  mHwnd = config.wHwnd;
  mWidth = config.wWdith;
  mHeight = config.wHeight;
  mResourcePath = config.resoucePath;

  mFrameCount = 2;
  mAspectRatio = static_cast<float>(mWidth) / static_cast<float>(mHeight);

  mFrameIndex = 0;
  mViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(mWidth), static_cast<float>(mHeight));
  mScissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(mWidth), static_cast<LONG>(mHeight));

  mRtvDescriptorSize = 0;
  mCBVDescriptorSize = 0;

  Build();

  return lRes;
}

void GraphicsCore::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter) {
  *ppAdapter = nullptr;

  ComPtr<IDXGIAdapter1> adapter;

  ComPtr<IDXGIFactory6> factory6;
  if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
    for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(
             adapterIndex, requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
             IID_PPV_ARGS(&adapter)));
         ++adapterIndex) {
      DXGI_ADAPTER_DESC1 desc;
      adapter->GetDesc1(&desc);

      if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
        // Don't select the Basic Render Driver adapter.
        // If you want a software adapter, pass in "/warp" on the command line.
        continue;
      }

      // Check to see whether the adapter supports Direct3D 12, but don't create the
      // actual device yet.
      if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
        break;
      }
    }
  }

  if (adapter.Get() == nullptr) {
    for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex) {
      DXGI_ADAPTER_DESC1 desc;
      adapter->GetDesc1(&desc);

      if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
        // Don't select the Basic Render Driver adapter.
        // If you want a software adapter, pass in "/warp" on the command line.
        continue;
      }

      // Check to see whether the adapter supports Direct3D 12, but don't create the
      // actual device yet.
      if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
        break;
      }
    }
  }

  *ppAdapter = adapter.Detach();
}

std::wstring GraphicsCore::GetResourceFilePath(const std::wstring& fileName) const {
  return mResourcePath + fileName;
}

void GraphicsCore::Build() {
  LoadCoreInterface();
  LoadPipeline();
}
void GraphicsCore::Update() {
  mCamera.UpdateViewProj();

  mVPBufferData.mView = mCamera.GetViewMatrix();
  mVPBufferData.mProj = mCamera.GetProjMatrix();

  memcpy(mVPBufferBegin, &mVPBufferData, sizeof(mVPBufferData));

  {
    memset(&mIGBufferData, 0, sizeof(mIGBufferData));

    mIGBufferData.backgroundColor = Vector4f(0.24, 0.24, 0.24, 1.0);
    mIGBufferData.gridColor = Vector4f(0.33, 0.33, 0.33, 1.0);
    mIGBufferData.cameraPos = mCamera.GetPosition();
    mIGBufferData.screenResolution[0] = mWidth;
    mIGBufferData.screenResolution[1] = mHeight;
    mIGBufferData.gridSize = 1.0;
    mIGBufferData.lineWidth = 0.4;
    mIGBufferData.lineSoftness = 0.8;

    memcpy(mIGBufferBegin, &mIGBufferData, sizeof(mIGBufferData));
  }
}
void GraphicsCore::Render() {
  PopulateCommandList();

  ID3D12CommandList* ppCommandList[] = {mCommandList.Get()};
  mCommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

  REWinResultSuccess(mSwapChain->Present(1, 0));

  WaitForPreviousFrame();
}
void GraphicsCore::Destroy() {
  WaitForPreviousFrame();
  CloseHandle(mFenceEvent);
}

void GraphicsCore::LoadCoreInterface() {
  UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
  {
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
      debugController->EnableDebugLayer();

      dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
  }
#endif

  ComPtr<IDXGIFactory4> factory;
  REWinResultSuccess(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

  if (mUseWarpDevice) {
    ComPtr<IDXGIAdapter> warpAdapter;
    REWinResultSuccess(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

    REWinResultSuccess(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice)));
  } else {
    ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(factory.Get(), &hardwareAdapter, true);

    REWinResultSuccess(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice)));
  }

  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

  REWinResultSuccess(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

  DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
  swapChainDesc.BufferCount = mFrameCount;
  std::cout << "mFrameCount : " << mFrameCount << std::endl;
  swapChainDesc.Width = mWidth;
  swapChainDesc.Height = mHeight;
  swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swapChainDesc.SampleDesc.Count = 1;

  ComPtr<IDXGISwapChain1> swapChain;
  REWinResultSuccess(factory->CreateSwapChainForHwnd(mCommandQueue.Get(), mHwnd, &swapChainDesc, nullptr, nullptr, &swapChain));

  REWinResultSuccess(swapChain.As(&mSwapChain));
  mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

  {
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = mFrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    REWinResultSuccess(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap)));

    mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mCBVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  }

  {
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());

    mRenderTargets.resize(mFrameCount);
    for (UINT n = 0; n < mFrameCount; n++) {
      REWinResultSuccess(mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n])));
      mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
      rtvHandle.Offset(1, mRtvDescriptorSize);
    }
  }

  REWinResultSuccess(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));

  {
    REWinResultSuccess(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mCommandList)));
    REWinResultSuccess(mCommandList->Close());
  }

  {
    REWinResultSuccess(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
    mFenceValue = 1;

    mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (mFenceEvent == nullptr) {
      REWinResultSuccess(HRESULT_FROM_WIN32(GetLastError()));
    }
  }
}
void GraphicsCore::LoadPipeline() {
  CreateRootSignature();
  CreatePSO();
  CreateVertexBuffer();
  CreateConstantBuffer();

  WaitForPreviousFrame();
}

void GraphicsCore::PopulateCommandList() {
  REWinResultSuccess(mCommandAllocator->Reset());

  REWinResultSuccess(mCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get()));

  mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

  ID3D12DescriptorHeap* ppHeaps[] = {mCbvHeap.Get()};
  mCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
  mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart());

  D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = mCbvHeap->GetGPUDescriptorHandleForHeapStart();
  gpuHandle.ptr += mCBVDescriptorSize;
  mCommandList->SetGraphicsRootDescriptorTable(1, gpuHandle);

  mCommandList->RSSetViewports(1, &mViewport);
  mCommandList->RSSetScissorRects(1, &mScissorRect);

  auto resouceBarrier1 =
      CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
  mCommandList->ResourceBarrier(1, &resouceBarrier1);

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mFrameIndex, mRtvDescriptorSize);
  mCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

  const float clearColor[] = {0.0, 0.2f, 0.4f, 1.0f};
  mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
  mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
  mCommandList->DrawInstanced(3, 1, 0, 0);

  auto resouceBarrier2 =
      CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
  mCommandList->ResourceBarrier(1, &resouceBarrier2);

  REWinResultSuccess(mCommandList->Close());
}
void GraphicsCore::WaitForPreviousFrame() {
  const UINT fence = mFenceValue;
  REWinResultSuccess(mCommandQueue->Signal(mFence.Get(), fence));
  mFenceValue++;

  if (mFence->GetCompletedValue() < fence) {
    REWinResultSuccess(mFence->SetEventOnCompletion(fence, mFenceEvent));
    WaitForSingleObject(mFenceEvent, INFINITE);
  }

  mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
}

void GraphicsCore::CreateRootSignature() {
  D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

  featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

  if (FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
  }

  // 定义两个描述符范围，分别对应两个CBV
  CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
  CD3DX12_ROOT_PARAMETER1 rootParameters[2];

  // 第一个CBV：CameraConstants (register b0)
  ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
  rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

  // 第二个CBV：GridConstants (register b1)
  ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
  rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);

  D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
      D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

  CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
  rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

  ComPtr<ID3DBlob> signature;
  ComPtr<ID3DBlob> error;
  REWinResultSuccess(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
  REWinResultSuccess(mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));
}

void GraphicsCore::CreateConstantBuffer() {
  D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
  cbvHeapDesc.NumDescriptors = 2;
  cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  REWinResultSuccess(mDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap)));

  // view proj buffer
  {
    const UINT constantBufferSize = sizeof(ViewProjBuffer);

    auto heap_pro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buff_desc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

    REWinResultSuccess(mDevice->CreateCommittedResource(&heap_pro, D3D12_HEAP_FLAG_NONE, &buff_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                        IID_PPV_ARGS(&mVPBffer)));

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = mVPBffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = constantBufferSize;
    mDevice->CreateConstantBufferView(&cbvDesc, mCbvHeap->GetCPUDescriptorHandleForHeapStart());

    CD3DX12_RANGE readRange(0, 0);
    REWinResultSuccess(mVPBffer->Map(0, &readRange, reinterpret_cast<void**>(&mVPBufferBegin)));
    memcpy(mVPBufferBegin, &mVPBufferData, sizeof(mVPBufferData));
  }

  // grid param
  {
    const UINT constantBufferSize = sizeof(InfiniteGridBuffer);

    auto heap_pro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buff_desc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

    REWinResultSuccess(mDevice->CreateCommittedResource(&heap_pro, D3D12_HEAP_FLAG_NONE, &buff_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                        IID_PPV_ARGS(&mIGBuffer)));

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = mIGBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = constantBufferSize;

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mCbvHeap->GetCPUDescriptorHandleForHeapStart();
    cpuHandle.ptr += mCBVDescriptorSize;
    mDevice->CreateConstantBufferView(&cbvDesc, cpuHandle);

    CD3DX12_RANGE readRange(0, 0);
    REWinResultSuccess(mIGBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mIGBufferBegin)));
    memcpy(mIGBufferBegin, &mIGBufferData, sizeof(mIGBufferData));
  }
}

void GraphicsCore::CreatePSO() {
  ComPtr<ID3DBlob> vertexShader;
  ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
  UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
  UINT compileFlags = 0;
#endif

  std::wstring vertexShaderPath = GetResourceFilePath(L"InfiniteGrid.hlsl");
  std::wcout << "vertexShaderPath : " << vertexShaderPath << std::endl;
  REWinResultSuccess(D3DCompileFromFile(vertexShaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));

  std::wstring pixelShaderPath = GetResourceFilePath(L"InfiniteGrid.hlsl");
  REWinResultSuccess(D3DCompileFromFile(pixelShaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

  D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

  D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
  psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
  psoDesc.pRootSignature = mRootSignature.Get();
  psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
  psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
  psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
  psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
  psoDesc.DepthStencilState.DepthEnable = FALSE;
  psoDesc.DepthStencilState.StencilEnable = FALSE;
  psoDesc.SampleMask = UINT_MAX;
  psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
  psoDesc.NumRenderTargets = 1;
  psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
  psoDesc.SampleDesc.Count = 1;
  REWinResultSuccess(mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState)));
}
void GraphicsCore::CreateVertexBuffer() {
  const float maxFloat = std::numeric_limits<float>::max();

  float halfMax = maxFloat * 0.5f;

  // Vertex triangleVertices[] = {{-halfMax, -halfMax, 0.0f}, {halfMax, -halfMax, 0.0f}, {-halfMax, halfMax, 0.0f},
  //                              {halfMax, -halfMax, 0.0f},  {halfMax, halfMax, 0.0f},  {-halfMax, halfMax, 0.0f}};

  Vertex triangleVertices[] = {{0.0f, 0.25f * mAspectRatio, 0.0f}, {0.25f, -0.25f * mAspectRatio, 0.0f}, {-0.25f, -0.25f * mAspectRatio, 0.0f}};

  const UINT vertexBufferSize = sizeof(triangleVertices);

  auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
  auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

  REWinResultSuccess(mDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                                      nullptr, IID_PPV_ARGS(&mVertexBuffer)));

  UINT8* pVertexDataBegin;
  CD3DX12_RANGE readRange(0, 0);
  REWinResultSuccess(mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
  memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
  mVertexBuffer->Unmap(0, nullptr);

  mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
  mVertexBufferView.StrideInBytes = sizeof(Vertex);
  mVertexBufferView.SizeInBytes = vertexBufferSize;
}

Result GraphicsCore::BindScene(Scene* pScene) {
  Result lRes;
  return lRes;
}
Result GraphicsCore::UnBindScne() {
  Result lRes;
  return lRes;
}

Camera* GraphicsCore::GetCamera() {
  return &mCamera;
}

}  // namespace RE::Engine