#pragma once

#include "GraphicsObject.h"
#include "Engine/Core/Core.h"

namespace re::engine::render {
class CommandListManager;
class ContextManager {};

struct GCInitInfo {
  bool enableDebugLayer = true;
  bool enableGPUBasedValidation = true;
  bool enableDXGIDebugInfo = true;
  bool useWarpDriver = false;
  bool requireDXRSupport = false;
  D3D_FEATURE_LEVEL d3DFeatureLevel = D3D_FEATURE_LEVEL_11_0;
  bool setStablePowerState = true;

  int swapChainBufferCount = 3;
  int swapChainWidth = 1280;
  int swapChainHeigth = 720;
  DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
};

class RE_API GraphicsCore {
 public:
  GraphicsCore();
  virtual ~GraphicsCore();

  RE_FINLE ID3D12Device* GetDevice() { return m_Device.Get(); }
  RE_FINLE CommandListManager* GetCommandListManager() { return m_CommandListManager.get(); }
  RE_FINLE ContextManager* GetContextManager() { return m_ContextManager.get(); }
  RE_FINLE D3D_FEATURE_LEVEL GetFeatureLevel() { return m_D3DFeatureLevel; }

  bool IsDeviceNvidia();
  bool IsDeviceAMD();
  bool IsDeviceIntel();

  void Initialize(GCInitInfo info);
  void Release();

  template <typename T, typename... Args>
  T* Create(Args&&... args) {
    static_assert(std::is_base_of_v<GraphicsObject, T>, "T must be derived from GraphicsObject to be created via GraphicsCore.");
    T* res = Alloc::Create<T>(std::forward<Args>(args)...);
    if (res) {
      res->SetGraphicsCore(this);
    }
    return res;
  }

 private:
  Microsoft::WRL::ComPtr<ID3D12Device> m_Device{nullptr};
  Alloc::UniquePtr<CommandListManager> m_CommandListManager{nullptr};
  Alloc::UniquePtr<ContextManager> m_ContextManager{nullptr};
  D3D_FEATURE_LEVEL m_D3DFeatureLevel;

  bool m_bTypedUAVLoadSupport_R11G11B10_FLOAT{false};
  bool m_bTypedUAVLoadSupport_R16G16B16A16_FLOAT{false};
};

}  // namespace re::engine::render