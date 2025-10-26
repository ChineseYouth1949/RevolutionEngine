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

  Camera* GetCamera();

 private:
  static void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false);
  std::wstring GetResourceFilePath(const std::wstring& fileName) const;

 private:
  Camera mCamera;

  HWND mHwnd;
  UINT mWidth;
  UINT mHeight;

  float mAspectRatio;
  bool mUseWarpDevice;
  std::wstring mResourcePath;

  using Vertex = Vector3f;

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
  UINT mCBVDescriptorSize;

  ComPtr<ID3D12Resource> mVertexBuffer;
  D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;

  UINT mFrameIndex;
  HANDLE mFenceEvent;
  ComPtr<ID3D12Fence> mFence;
  UINT64 mFenceValue;

  void LoadCoreInterface();

  void WaitForPreviousFrame();

  void CreatePSO();
  void CreateVertexBuffer();

  struct ViewProjBuffer {
    Matrix4x4 mView;
    Matrix4x4 mProj;
    float padding[32];
  };
  static_assert((sizeof(ViewProjBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

  struct InfiniteGridBuffer {
    Vector4f backgroundColor;
    Vector4f gridColor;
    Vector3f cameraPos;
    float _padding1;
    int32_t screenResolution[2];
    float _padding2[2];
    float gridSize;
    float lineWidth;
    float lineSoftness;
    float _padding3[64 - 19];
  };
  static_assert((sizeof(InfiniteGridBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

  ComPtr<ID3D12DescriptorHeap> mCbvHeap;

  ComPtr<ID3D12Resource> mVPBffer;
  ViewProjBuffer mVPBufferData;
  UINT8* mVPBufferBegin;

  ComPtr<ID3D12Resource> mIGBuffer;
  InfiniteGridBuffer mIGBufferData;
  UINT8* mIGBufferBegin;

  void LoadPipeline();
  void PopulateCommandList();

  void CreateRootSignature();
  void CreateConstantBuffer();
};

}  // namespace RE::Engine