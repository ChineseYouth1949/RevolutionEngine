// #include "GraphicCore.h"

// #include "DXHelper.h"

// namespace Engine {

// GraphicCore::GraphicCore() {
//   m_useWarpDevice = false;
// }

// void GraphicCore::Init() {
//   UINT dxgiFactoryFlags = 0;

// #if defined(_DEBUG)
//   {
//     ComPtr<ID3D12Debug> debugController;
//     if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
//       debugController->EnableDebugLayer();

//       dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//     }
//   }
// #endif

//   ComPtr<IDXGIFactory4> factory;
//   ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

//   if (m_useWarpDevice) {
//     ComPtr<IDXGIAdapter> warpAdapter;
//     ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

//     ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
//   } else {
//     ComPtr<IDXGIAdapter1> hardwareAdapter;

//     for (UINT i = 0; factory->EnumAdapters1(i, &hardwareAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
//       DXGI_ADAPTER_DESC1 desc;
//       hardwareAdapter->GetDesc1(&desc);

//       std::wcout << L"适配器 #" << i << L": " << desc.Description << L"\n";
//       std::wcout << L"  设备ID: 0x" << std::hex << desc.DeviceId << "\n";
//       std::wcout << L"  专用显存: " << std::dec << desc.DedicatedVideoMemory / 1024 / 1024 << " MB\n";
//       std::wcout << L"  共享系统内存: " << desc.SharedSystemMemory / 1024 / 1024 << " MB\n\n";
//     }

//     ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
//   }

//   D3D12_COMMAND_QUEUE_DESC queueDesc = {};
//   queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//   queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

//   ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
// }

// }  // namespace Engine