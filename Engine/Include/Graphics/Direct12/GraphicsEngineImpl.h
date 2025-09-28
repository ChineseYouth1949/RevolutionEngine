#pragma once

#include <vector>

#include "D3D12.h"

namespace RE::Core {

struct RecordResource {
  ComPtr<ID3D12CommandAllocator> commandAllocator;
  ComPtr<ID3D12GraphicsCommandList> commandList;
};

struct FrameResource {
  ID3D12Resource renderTarget;
  UINT64 fenceValue;

  std::shared_ptr<RecordResource> recordResources;
};

class RE_API GraphicsEngineImpl {
 public:
  GraphicsEngineImpl();
  ~GraphicsEngineImpl();

  GraphicsEngineImpl(GraphicsEngineImpl&&) noexcept = default;
  GraphicsEngineImpl& operator=(GraphicsEngineImpl&&) noexcept = default;

  GraphicsEngineImpl(const GraphicsEngineImpl&) = delete;
  GraphicsEngineImpl& operator=(const GraphicsEngineImpl&) = delete;

  Result ConfigOptions();
  Result Build();

  Result LoadScene();

 private:
  ComPtr<ID3D12Device> m_device;
  ComPtr<IDXGISwapChain3> m_swapChain;

  uint8_t m_frameCount;
  std::vector<FrameResource> m_frameResouces;
};

}  // namespace RE::Core