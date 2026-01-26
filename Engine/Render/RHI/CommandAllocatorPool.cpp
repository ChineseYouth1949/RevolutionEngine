#include "GraphicsCore.h"
#include "CommandAllocatorPool.h"

namespace re::engine::render {

CommandAllocatorPool::CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE Type) : m_cCommandListType(Type) {}

CommandAllocatorPool::~CommandAllocatorPool() {
  Release();
}

void CommandAllocatorPool::Release() {
  for (size_t i = 0; i < m_AllocatorPool.size(); ++i)
    m_AllocatorPool[i]->Release();

  m_AllocatorPool.clear();
}

ID3D12CommandAllocator* CommandAllocatorPool::RequestAllocator(uint64_t CompletedFenceValue) {
  std::lock_guard<std::mutex> LockGuard(m_AllocatorMutex);

  ID3D12CommandAllocator* pAllocator = nullptr;

  if (!m_ReadyAllocators.empty()) {
    stl::pair<uint64_t, ID3D12CommandAllocator*>& AllocatorPair = m_ReadyAllocators.front();

    if (AllocatorPair.first <= CompletedFenceValue) {
      pAllocator = AllocatorPair.second;
      RE_ASSERT_SUCCEEDED(pAllocator->Reset());
      m_ReadyAllocators.pop();
    }
  }

  // If no allocator's were ready to be reused, create a new one
  if (pAllocator == nullptr) {
    RE_ASSERT_SUCCEEDED(RE_GCDevice->CreateCommandAllocator(m_cCommandListType, IID_PPV_ARGS(&pAllocator)));
    wchar_t AllocatorName[32];
    swprintf(AllocatorName, 32, L"CommandAllocator %zu", m_AllocatorPool.size());
    pAllocator->SetName(AllocatorName);
    m_AllocatorPool.push_back(pAllocator);
  }

  return pAllocator;
}

void CommandAllocatorPool::DiscardAllocator(uint64_t FenceValue, ID3D12CommandAllocator* Allocator) {
  std::lock_guard<std::mutex> LockGuard(m_AllocatorMutex);

  // That fence value indicates we are free to reset the allocator
  m_ReadyAllocators.push(stl::make_pair(FenceValue, Allocator));
}

}  // namespace re::engine::render