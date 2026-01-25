#pragma once

namespace re::engine::render {
class CommandListManager;
class ContextManager {};

class GraphicsCoreBase {
 public:
  RE_FINLE ID3D12Device* GetDevice() { return m_Device.Get(); }
  RE_FINLE CommandListManager* GetCommandListManager() { return m_CommandListManager.get(); }
  RE_FINLE ContextManager* GetContextManager() { return m_ContextManager.get(); }
  RE_FINLE D3D_FEATURE_LEVEL GetFeatureLevel() { return m_D3DFeatureLevel; }

  bool IsDeviceNvidia();
  bool IsDeviceAMD();
  bool IsDeviceIntel();

 protected:
  GraphicsCoreBase();
  virtual ~GraphicsCoreBase();

  struct InitInfo {
    bool enableDebugLayer = true;
    bool enableGPUBasedValidation = true;
    bool enableDXGIDebugInfo = true;
    bool useWarpDriver = false;
    bool requireDXRSupport = false;
    D3D_FEATURE_LEVEL d3DFeatureLevel = D3D_FEATURE_LEVEL_11_0;
    bool setStablePowerState = true;
  };

  void Initialize(InitInfo info);
  void Release();

 private:
  Microsoft::WRL::ComPtr<ID3D12Device> m_Device{nullptr};
  stl::unique_ptr<CommandListManager> m_CommandListManager{nullptr};
  stl::unique_ptr<ContextManager> m_ContextManager{nullptr};
  D3D_FEATURE_LEVEL m_D3DFeatureLevel;

  bool m_bTypedUAVLoadSupport_R11G11B10_FLOAT{false};
  bool m_bTypedUAVLoadSupport_R16G16B16A16_FLOAT{false};
};

}  // namespace re::engine::render