#pragma once

#include "CommandListManager.h"
#include "Engine/Core/Memory/Allocator.h"

namespace re::engine::render {
using Microsoft::WRL::ComPtr;

struct GraphicsCoreCreateInfo {
  bool enableDebugLayer = true;
  bool useWarpDevice = false;
  int swapChainBufferCount = 3;
  int swapChainWidth;
  int swapChainHeight;
  DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
  DXGI_SWAP_EFFECT swapChainEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  DXGI_SCALING swapChainScaling = DXGI_SCALING_NONE;
  UINT swapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
  HWND swapChainHwnd;
};

class GraphicsCore {
 public:
  RE_CLASS_ALLOCATOR(MiMalloc)

  GraphicsCore();
  ~GraphicsCore();

  void Create(GraphicsCoreCreateInfo info);
  void OnRender();

 protected:
  void PopulateCommandList();

  ComPtr<IDXGIFactory4> m_Factory;
  ComPtr<IDXGIAdapter> m_Adapter;
  ComPtr<ID3D12Device> m_Device;

  ClassAllocator::UniquePtr<CommandListManager> m_CommandListManager;
  ComPtr<IDXGISwapChain3> m_SwapChain;
  UINT m_FrameIndex;

  ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
  UINT m_RtvDescriptorSize;
  stl::vector<ComPtr<ID3D12Resource>> m_RenderTargets;

  ComPtr<ID3D12GraphicsCommandList> m_CommandList;
  ID3D12CommandAllocator* m_CommandAllocator = nullptr;
};
}  // namespace re::engine::render