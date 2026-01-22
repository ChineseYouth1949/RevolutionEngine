#pragma once

namespace re::engine::render {
class DXCore {
 public:
  void Init() {
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
  }
};
}  // namespace re::engine::render