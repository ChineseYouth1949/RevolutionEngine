#pragma once

#include "GraphicsObject.h"

namespace re::engine::render {

class CommandAllocatorPool : public GraphicsObject {
 public:
  CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE Type);
  ~CommandAllocatorPool();

  void Release();

  ID3D12CommandAllocator* RequestAllocator(uint64_t CompletedFenceValue);
  void DiscardAllocator(uint64_t FenceValue, ID3D12CommandAllocator* Allocator);

  RE_FINLE size_t Size() { return m_AllocatorPool.size(); }

 private:
  const D3D12_COMMAND_LIST_TYPE m_cCommandListType;

  Alloc::vector<ID3D12CommandAllocator*> m_AllocatorPool;
  Alloc::queue<stl::pair<uint64_t, ID3D12CommandAllocator*>> m_ReadyAllocators;
  std::mutex m_AllocatorMutex;
};
}  // namespace re::engine::render