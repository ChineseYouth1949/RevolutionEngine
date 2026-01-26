#pragma once

#include "GraphicsObject.h"
#include "RootSignature.h"
#include "PipelineState.h"

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
  int swapChainHeight = 720;
  DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
  DXGI_SWAP_EFFECT swapChainEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  DXGI_SCALING swapChainScaling = DXGI_SCALING_NONE;
  UINT swapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
  HWND swapChainHWND;
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

  void SetAssetPath(const Alloc::wstring& assertPath) { m_AssetsPath = assertPath; }
  Alloc::wstring GetAssetFullPath(const Alloc::wstring& assetName);

  DXGI_FORMAT GetSwapChainForamt() {
    DXGI_SWAP_CHAIN_DESC1 desc;
    HRESULT hr = m_SwapChain->GetDesc1(&desc);
    if (SUCCEEDED(hr)) {
      DXGI_FORMAT backBufferFormat = desc.Format;  // 这里就是后台缓冲区格式
    }
    return desc.Format;
  }

  void Initialize(GCInitInfo info);
  void Release();

  void Begin();
  void End();
  ID3D12GraphicsCommandList* GetCommandList() { return m_CommandList; }

 private:
  Microsoft::WRL::ComPtr<ID3D12Device> m_Device{nullptr};
  Alloc::UniquePtr<CommandListManager> m_CommandListManager{nullptr};

  ID3D12GraphicsCommandList* m_CommandList{nullptr};
  ID3D12CommandAllocator* m_CommandAllocator{nullptr};

  ComPtr<IDXGISwapChain3> m_SwapChain;
  UINT m_FrameIndex;
  ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
  UINT m_RtvDescriptorSize;
  Alloc::vector<ComPtr<ID3D12Resource>> m_RenderTargets;
  ComPtr<ID3D12Fence> m_Fence;
  UINT64 m_FrameFenceValues[10];

  CD3DX12_VIEWPORT m_Viewport;
  CD3DX12_RECT m_ScissorRect;

  Alloc::UniquePtr<ContextManager> m_ContextManager{nullptr};
  D3D_FEATURE_LEVEL m_D3DFeatureLevel;

  bool m_bTypedUAVLoadSupport_R11G11B10_FLOAT{false};
  bool m_bTypedUAVLoadSupport_R16G16B16A16_FLOAT{false};

  Alloc::wstring m_AssetsPath;
};

}  // namespace re::engine::render