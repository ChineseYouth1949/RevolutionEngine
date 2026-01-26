#pragma once

#include "GraphicsObject.h"
#include "CommandQueue.h"

namespace re::engine::render {

class CommandListManager : public GraphicsObject {
  friend class CommandContext;

 public:
  CommandListManager();
  ~CommandListManager();

  void Initialize();
  void Release();

  CommandQueue& GetGraphicsQueue(void) { return m_GraphicsQueue; }
  CommandQueue& GetComputeQueue(void) { return m_ComputeQueue; }
  CommandQueue& GetCopyQueue(void) { return m_CopyQueue; }

  CommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT) {
    switch (Type) {
      case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        return m_ComputeQueue;
      case D3D12_COMMAND_LIST_TYPE_COPY:
        return m_CopyQueue;
      default:
        return m_GraphicsQueue;
    }
  }

  void CreateNewCommandList(D3D12_COMMAND_LIST_TYPE Type, ID3D12GraphicsCommandList** List, ID3D12CommandAllocator** Allocator);

  // Test to see if a fence has already been reached
  bool IsFenceComplete(uint64_t FenceValue) { return GetQueue(D3D12_COMMAND_LIST_TYPE(FenceValue >> 56)).IsFenceComplete(FenceValue); }

  // The CPU will wait for a fence to reach a specified value
  void WaitForFence(uint64_t FenceValue);

  // The CPU will wait for all command queues to empty (so that the GPU is idle)
  void IdleGPU(void) {
    m_GraphicsQueue.WaitForIdle();
    m_ComputeQueue.WaitForIdle();
    m_CopyQueue.WaitForIdle();
  }

 private:
  CommandQueue m_GraphicsQueue;
  CommandQueue m_ComputeQueue;
  CommandQueue m_CopyQueue;
};

}  // namespace re::engine::render