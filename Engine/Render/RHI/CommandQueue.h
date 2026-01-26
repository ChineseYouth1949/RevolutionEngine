#pragma once

#include "GraphicsObject.h"
#include "CommandAllocatorPool.h"

namespace re::engine::render {

class CommandQueue : public GraphicsObject {
  friend class CommandListManager;
  friend class CommandContext;

 public:
  CommandQueue(D3D12_COMMAND_LIST_TYPE Type);
  ~CommandQueue();

  void Initialize();
  void Release();

  RE_FINLE bool IsReady() { return m_CommandQueue != nullptr; }

  uint64_t IncrementFence(void);
  bool IsFenceComplete(uint64_t FenceValue);
  void StallForFence(CommandQueue& Producer, uint64_t FenceValue);
  void StallForProducer(CommandQueue& Producer);
  void WaitForFence(uint64_t FenceValue);
  void WaitForIdle(void) { WaitForFence(IncrementFence()); }

  ID3D12CommandQueue* GetCommandQueue() { return m_CommandQueue; }

  uint64_t GetNextFenceValue() { return m_NextFenceValue; }

  uint64_t ExecuteCommandList(ID3D12CommandList* List);
  ID3D12CommandAllocator* RequestAllocator(void);
  void DiscardAllocator(uint64_t FenceValueForReset, ID3D12CommandAllocator* Allocator);

 private:
  ID3D12CommandQueue* m_CommandQueue;

  const D3D12_COMMAND_LIST_TYPE m_Type;

  CommandAllocatorPool m_AllocatorPool;
  std::mutex m_FenceMutex;
  std::mutex m_EventMutex;

  // Lifetime of these objects is managed by the descriptor cache
  ID3D12Fence* m_pFence;
  uint64_t m_NextFenceValue;
  uint64_t m_LastCompletedFenceValue;
  HANDLE m_FenceEventHandle;
};

}  // namespace re::engine::render