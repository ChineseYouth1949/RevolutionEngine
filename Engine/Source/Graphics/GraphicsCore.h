#pragma once

#include "Base/CompileDefine.h"
#include "Base/Result.h"

#include "Object/RObject.h"
#include "Utils/Utils.h"
#include "Scene/Scene.h"

#include "Graphics/Graphics.h"

#include "D3D12.h"

#include <vector>
#include <string>
#include <iostream>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

namespace RE::Engine {

class GraphicsCore : public RObject {
 public:
  GraphicsCore();
  ~GraphicsCore();

  GraphicsCore(GraphicsCore&&) noexcept = default;
  GraphicsCore& operator=(GraphicsCore&&) noexcept = default;

  GraphicsCore(const GraphicsCore&) = delete;
  GraphicsCore& operator=(const GraphicsCore&) = delete;

  Result Initialize(const GraphicsCoreConfig& config);

  void Build();
  void Update();
  void Render();
  void Destroy();

  Result BindScene(Scene* pScene);
  Result UnBindScne();

 private:
  static void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false);
  std::wstring GetResourceFilePath(const std::wstring& fileName) const;

 private:
  HWND mHwnd;
  UINT mWidth;
  UINT mHeight;

  float mAspectRatio;
  bool mUseWarpDevice;
  std::wstring mResourcePath;

  struct Vertex {
    XMFLOAT3 position;
    XMFLOAT4 color;
  };

  UINT mFrameCount;

  CD3DX12_VIEWPORT mViewport;
  CD3DX12_RECT mScissorRect;

  ComPtr<IDXGISwapChain3> mSwapChain;
  ComPtr<ID3D12Device> mDevice;

  std::vector<ComPtr<ID3D12Resource>> mRenderTargets;
  ComPtr<ID3D12CommandAllocator> mCommandAllocator;

  ComPtr<ID3D12CommandQueue> mCommandQueue;
  ComPtr<ID3D12RootSignature> mRootSignature;
  ComPtr<ID3D12DescriptorHeap> mRtvHeap;
  ComPtr<ID3D12PipelineState> mPipelineState;
  ComPtr<ID3D12GraphicsCommandList> mCommandList;
  UINT mRtvDescriptorSize;

  ComPtr<ID3D12Resource> mVertexBuffer;
  D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;

  UINT mFrameIndex;
  HANDLE mFenceEvent;
  ComPtr<ID3D12Fence> mFence;
  UINT64 mFenceValue;

  void LoadCoreInterface();
  void LoadPipeline();

  void PopulateCommandList();
  void WaitForPreviousFrame();

  void CreateRootSignature();
  void CreatePSO();
  void CreateVertexBuffer();
};

}  // namespace RE::Engine