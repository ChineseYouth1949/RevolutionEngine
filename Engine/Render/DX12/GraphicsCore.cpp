// #include "Engine/Render/DX12/GraphicsCore.h"

// namespace re::engine::render {
// void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter) {
//   *ppAdapter = nullptr;

//   ComPtr<IDXGIAdapter1> adapter;

//   ComPtr<IDXGIFactory6> factory6;
//   if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
//     for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(
//              adapterIndex, requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
//              IID_PPV_ARGS(&adapter)));
//          ++adapterIndex) {
//       DXGI_ADAPTER_DESC1 desc;
//       adapter->GetDesc1(&desc);

//       if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
//         // Don't select the Basic Render Driver adapter.
//         // If you want a software adapter, pass in "/warp" on the command line.
//         continue;
//       }

//       // Check to see whether the adapter supports Direct3D 12, but don't create the
//       // actual device yet.
//       if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
//         break;
//       }
//     }
//   }

//   if (adapter.Get() == nullptr) {
//     for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex) {
//       DXGI_ADAPTER_DESC1 desc;
//       adapter->GetDesc1(&desc);

//       if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
//         // Don't select the Basic Render Driver adapter.
//         // If you want a software adapter, pass in "/warp" on the command line.
//         continue;
//       }

//       // Check to see whether the adapter supports Direct3D 12, but don't create the
//       // actual device yet.
//       if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
//         break;
//       }
//     }
//   }

//   *ppAdapter = adapter.Detach();
// }

// void GraphicsCore::Create(GraphicsCoreCreateInfo info) {
//   UINT dxgiFactoryFlags = 0;

//   // Check enabel debug layer
//   if (info.enableDebugLayer) {
//     ComPtr<ID3D12Debug> debugController;
//     if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
//       debugController->EnableDebugLayer();

//       dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//     }
//   }

//   // Create factory
//   ComPtr<IDXGIFactory4> factory;
//   RE_ASSERT_HR(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

//   if (info.useWarpDevice) {
//     ComPtr<IDXGIAdapter> warpAdapter;
//     RE_ASSERT_HR(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

//     RE_ASSERT_HR(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
//   } else {
//     ComPtr<IDXGIAdapter1> hardwareAdapter;
//     GetHardwareAdapter(factory.Get(), &hardwareAdapter, true);

//     RE_ASSERT_HR(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
//   }

//   // CommandListManager
//   m_CommandListManager = ClassAllocator::CreateUniquePtr<CommandListManager>();
//   m_CommandListManager->Create(m_Device.Get());

//   m_CommandListManager->CreateNewCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, &m_CommandList, &m_CommandAllocator);
//   m_CommandListManager->GetGraphicsQueue().

//   // Create swap chain
//   DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
//   swapChainDesc.BufferCount = info.swapChainBufferCount;
//   swapChainDesc.Width = info.swapChainWidth;
//   swapChainDesc.Height = info.swapChainHeight;
//   swapChainDesc.Format = info.swapChainFormat;
//   swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//   swapChainDesc.SwapEffect = info.swapChainEffect;
//   swapChainDesc.SampleDesc.Count = 1;
//   swapChainDesc.Scaling = info.swapChainScaling;
//   swapChainDesc.Flags = info.swapChainFlags;

//   ComPtr<IDXGISwapChain1> swapChain;
//   RE_ASSERT_HR(factory->CreateSwapChainForHwnd(m_CommandListManager->GetGraphicsQueue().GetCommandQueue(), info.swapChainHwnd, &swapChainDesc,
//                                                nullptr, nullptr, &swapChain));

//   // initlize m_FrameIndex
//   RE_ASSERT_HR(swapChain.As(&m_SwapChain));
//   m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

//   // Create descriptor heaps.
//   {
//     // Describe and create a render target view (RTV) descriptor heap.
//     D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
//     rtvHeapDesc.NumDescriptors = info.swapChainBufferCount;
//     rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//     rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//     RE_ASSERT_HR(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap)));

//     m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//   }

//   // Create frame resources.
//   {
//     CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());

//     // Create a RTV and a command allocator for each frame.
//     m_RenderTargets.resize(info.swapChainBufferCount);
//     for (UINT n = 0; n < info.swapChainBufferCount; n++) {
//       RE_ASSERT_HR(m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_RenderTargets[n])));
//       m_Device->CreateRenderTargetView(m_RenderTargets[n].Get(), nullptr, rtvHandle);
//       rtvHandle.Offset(1, m_RtvDescriptorSize);
//     }
//   }
// }

// void GraphicsCore::PopulateCommandList() {
//   // Command list allocators can only be reset when the associated
//   // command lists have finished execution on the GPU; apps should use
//   // fences to determine GPU execution progress.

//   RE_ASSERT_HR(m_commandAllocators[m_frameIndex]->Reset());

//   // However, when ExecuteCommandList() is called on a particular command
//   // list, that command list can then be reset at any time and must be before
//   // re-recording.
//   RE_ASSERT_HR(m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), m_pipelineState.Get()));

//   // Set necessary state.
//   m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
//   m_commandList->RSSetViewports(1, &m_viewport);
//   m_commandList->RSSetScissorRects(1, &m_scissorRect);

//   // Indicate that the back buffer will be used as a render target.
//   m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
//                                                                           D3D12_RESOURCE_STATE_RENDER_TARGET));

//   CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
//   m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

//   // Record commands.
//   const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
//   m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
//   m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//   m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
//   m_commandList->DrawInstanced(3, 1, 0, 0);

//   // Indicate that the back buffer will now be used to present.
//   m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
//                                                                           D3D12_RESOURCE_STATE_PRESENT));

//   RE_ASSERT_HR(m_commandList->Close());
// }

// void GraphicsCore::OnRender() {}

// }  // namespace re::engine::render