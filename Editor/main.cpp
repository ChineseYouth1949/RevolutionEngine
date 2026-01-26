#include <QApplication>

#include <Engine/Core/Core.h>

#include "mainwindow.h"

RE_EXPORT_D3D12_AGILITY_SDK

using Microsoft::WRL::ComPtr;

void InitD3D12(ComPtr<ID3D12Device>& outDevice) {
  uint32_t dxgiFactoryFlags = 0;

  // 1. 开启调试层 (Debug Layer)
#ifdef RE_DEBUG
  ComPtr<ID3D12Debug> debugController;
  if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
    debugController->EnableDebugLayer();
    // 开启 GPU 辅助验证 (可选)
    // debugController->SetEnableGPUBasedValidation(true);
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
  }
#endif

  // 2. 创建 DXGI Factory
  ComPtr<IDXGIFactory6> factory;
  RE_ASSERT_SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

  // 3. 枚举高性能 GPU 适配器 (Adapter)
  ComPtr<IDXGIAdapter1> adapter;
  for (uint32_t adapterIndex = 0;
       DXGI_ERROR_NOT_FOUND != factory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
       ++adapterIndex) {
    DXGI_ADAPTER_DESC1 desc;
    adapter->GetDesc1(&desc);

    // 跳过软件模拟设备 (WARP)
    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
      continue;

    // 4. 尝试创建设备
    // D3D_FEATURE_LEVEL_12_0 是 2026 年现代引擎的基准
    HRESULT hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&outDevice));

    if (SUCCEEDED(hr)) {
      // 设置调试名称方便 PIX/RenderDoc 查看
      outDevice->SetName(L"RE_Primary_Device");
      break;
    }
  }

  RE_ASSERT(outDevice != nullptr, "Failed to create D3D12 Device!");
}

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("REditor");
  app.setApplicationVersion("1.0");
  app.setOrganizationName("Revolution");

  re::engine::ecs::World world;
  ComPtr<ID3D12Device> outDevice;
  InitD3D12(outDevice);

  re::editor::MainWindow window;
  window.resize(800, 600);
  window.show();

  return app.exec();
}