#pragma once

#include "Engine/Base/All.h"

#include "Engine/Render/RHI/GraphicsCore.h"
#include "Engine/Render/RHI/SystemTime.h"
#include "Engine/Render/RHI/TextRenderer.h"
#include "Engine/Render/RHI/CommandContext.h"
#include "Engine/Render/RHI/RootSignature.h"
#include "Engine/Render/RHI/PipelineState.h"
#include "Engine/Render/RHI/BufferManager.h"

namespace re::engine::render {
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
  int swapChainHeight = 720;
  DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
  DXGI_SWAP_EFFECT swapChainEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  DXGI_SCALING swapChainScaling = DXGI_SCALING_NONE;
  UINT swapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
  HWND swapChainHWND;
};

using GraphicsContext = ::GraphicsContext;

class RE_API GraphicsCore {
 public:
  GraphicsCore();
  virtual ~GraphicsCore();

  void Initialize(GCInitInfo info);
  void Release();

  RE_FINLINE ID3D12Device* GetDevice() { return m_Device.Get(); }
  RE_FINLINE GraphicsContext* GetGraphicsContext() { return m_GraphicsContext; }

  bool IsDeviceNvidia();
  bool IsDeviceAMD();
  bool IsDeviceIntel();

  void SetResourcePath(const wstring& assertPath) { m_AssetsPath = assertPath; }
  wstring GetResourceFilePath(const wstring& assetName);

  void Begin();
  void End();

 private:
  Microsoft::WRL::ComPtr<ID3D12Device> m_Device;

  D3D_FEATURE_LEVEL m_D3DFeatureLevel;
  bool m_bTypedUAVLoadSupport_R11G11B10_FLOAT{false};
  bool m_bTypedUAVLoadSupport_R16G16B16A16_FLOAT{false};

  GraphicsContext* m_GraphicsContext = nullptr;

  wstring m_AssetsPath;
};

}  // namespace re::engine::render