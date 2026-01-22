#include "CommandQueue.h"

namespace re::engine::render {

CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE Type)
    : m_Type(Type),
      m_CommandQueue(nullptr),
      m_pFence(nullptr),
      m_NextFenceValue((uint64_t)Type << 56 | 1),
      m_LastCompletedFenceValue((uint64_t)Type << 56),
      m_AllocatorPool(Type) {}

CommandQueue::~CommandQueue() {
  Shutdown();
}

void CommandQueue::Shutdown() {
  if (m_CommandQueue == nullptr)
    return;

  m_AllocatorPool.Shutdown();

  CloseHandle(m_FenceEventHandle);

  m_pFence->Release();
  m_pFence = nullptr;

  m_CommandQueue->Release();
  m_CommandQueue = nullptr;
}

void CommandQueue::Create(ID3D12Device* pDevice) {
  RE_ASSERT(pDevice != nullptr);
  RE_ASSERT(!IsReady());
  RE_ASSERT(m_AllocatorPool.Size() == 0);

  D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
  QueueDesc.Type = m_Type;
  QueueDesc.NodeMask = 1;
  pDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&m_CommandQueue));
  m_CommandQueue->SetName(L"CommandListManager::m_CommandQueue");

  RE_ASSERT_HR(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)));
  m_pFence->SetName(L"CommandListManager::m_pFence");
  m_pFence->Signal((uint64_t)m_Type << 56);

  m_FenceEventHandle = CreateEvent(nullptr, false, false, nullptr);
  RE_ASSERT(m_FenceEventHandle != NULL);

  m_AllocatorPool.Create(pDevice);

  RE_ASSERT(IsReady());
}

uint64_t CommandQueue::ExecuteCommandList(ID3D12CommandList* List) {
  std::lock_guard<std::mutex> LockGuard(m_FenceMutex);

  RE_ASSERT_HR(((ID3D12GraphicsCommandList*)List)->Close());

  // Kickoff the command list
  m_CommandQueue->ExecuteCommandLists(1, &List);

  // Signal the next fence value (with the GPU)
  m_CommandQueue->Signal(m_pFence, m_NextFenceValue);

  // And increment the fence value.
  return m_NextFenceValue++;
}

uint64_t CommandQueue::IncrementFence(void) {
  std::lock_guard<std::mutex> LockGuard(m_FenceMutex);
  m_CommandQueue->Signal(m_pFence, m_NextFenceValue);
  return m_NextFenceValue++;
}

bool CommandQueue::IsFenceComplete(uint64_t FenceValue) {
  // Avoid querying the fence value by testing against the last one seen.
  // The max() is to protect against an unlikely race condition that could cause the last
  // completed fence value to regress.
  if (FenceValue > m_LastCompletedFenceValue)
    m_LastCompletedFenceValue = std::max(m_LastCompletedFenceValue, m_pFence->GetCompletedValue());

  return FenceValue <= m_LastCompletedFenceValue;
}

void CommandQueue::StallForFence(CommandQueue& Producer, uint64_t FenceValue) {
  m_CommandQueue->Wait(Producer.m_pFence, FenceValue);
}

void CommandQueue::StallForProducer(CommandQueue& Producer) {
  RE_ASSERT(Producer.m_NextFenceValue > 0);
  m_CommandQueue->Wait(Producer.m_pFence, Producer.m_NextFenceValue - 1);
}

void CommandQueue::WaitForFence(uint64_t FenceValue) {
  if (IsFenceComplete(FenceValue))
    return;

  // TODO:  Think about how this might affect a multi-threaded situation.  Suppose thread A
  // wants to wait for fence 100, then thread B comes along and wants to wait for 99.  If
  // the fence can only have one event set on completion, then thread B has to wait for
  // 100 before it knows 99 is ready.  Maybe insert sequential events?
  {
    std::lock_guard<std::mutex> LockGuard(m_EventMutex);

    m_pFence->SetEventOnCompletion(FenceValue, m_FenceEventHandle);
    WaitForSingleObject(m_FenceEventHandle, INFINITE);
    m_LastCompletedFenceValue = FenceValue;
  }
}

ID3D12CommandAllocator* CommandQueue::RequestAllocator() {
  uint64_t CompletedFence = m_pFence->GetCompletedValue();

  return m_AllocatorPool.RequestAllocator(CompletedFence);
}

void CommandQueue::DiscardAllocator(uint64_t FenceValue, ID3D12CommandAllocator* Allocator) {
  m_AllocatorPool.DiscardAllocator(FenceValue, Allocator);
}

}  // namespace re::engine::render