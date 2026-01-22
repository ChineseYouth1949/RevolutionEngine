#pragma once

namespace re::engine::render {
using namespace core;

class CommandAllocatorPool {
 public:
  CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE Type);
  ~CommandAllocatorPool();

  void Create(ID3D12Device* pDevice);
  void Shutdown();

  ID3D12CommandAllocator* RequestAllocator(uint64_t CompletedFenceValue);
  void DiscardAllocator(uint64_t FenceValue, ID3D12CommandAllocator* Allocator);

  RE_FINLE size_t Size() { return m_AllocatorPool.size(); }

 private:
  const D3D12_COMMAND_LIST_TYPE m_cCommandListType;

  ID3D12Device* m_Device;
  stl::vector<ID3D12CommandAllocator*> m_AllocatorPool;
  stl::queue<stl::pair<uint64_t, ID3D12CommandAllocator*>> m_ReadyAllocators;
  std::mutex m_AllocatorMutex;
};
}  // namespace re::engine::render