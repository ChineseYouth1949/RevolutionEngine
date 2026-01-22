#include "CommandListManager.h"

namespace re::engine::render {

CommandListManager::CommandListManager()
    : m_Device(nullptr),
      m_GraphicsQueue(D3D12_COMMAND_LIST_TYPE_DIRECT),
      m_ComputeQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE),
      m_CopyQueue(D3D12_COMMAND_LIST_TYPE_COPY) {}

CommandListManager::~CommandListManager() {
  Shutdown();
}

void CommandListManager::Shutdown() {
  m_GraphicsQueue.Shutdown();
  m_ComputeQueue.Shutdown();
  m_CopyQueue.Shutdown();
}

void CommandListManager::Create(ID3D12Device* pDevice) {
  RE_ASSERT(pDevice != nullptr);

  m_Device = pDevice;

  m_GraphicsQueue.Create(pDevice);
  m_ComputeQueue.Create(pDevice);
  m_CopyQueue.Create(pDevice);
}

void CommandListManager::CreateNewCommandList(D3D12_COMMAND_LIST_TYPE Type, ID3D12GraphicsCommandList** List, ID3D12CommandAllocator** Allocator) {
  RE_ASSERT_MSG(Type != D3D12_COMMAND_LIST_TYPE_BUNDLE, "Bundles are not yet supported");
  switch (Type) {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
      *Allocator = m_GraphicsQueue.RequestAllocator();
      break;
    case D3D12_COMMAND_LIST_TYPE_BUNDLE:
      break;
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
      *Allocator = m_ComputeQueue.RequestAllocator();
      break;
    case D3D12_COMMAND_LIST_TYPE_COPY:
      *Allocator = m_CopyQueue.RequestAllocator();
      break;
  }

  RE_ASSERT_HR(m_Device->CreateCommandList(1, Type, *Allocator, nullptr, IID_PPV_ARGS(List)));
  (*List)->SetName(L"CommandList");
}

void CommandListManager::WaitForFence(uint64_t FenceValue) {
  CommandQueue& Producer = GetQueue((D3D12_COMMAND_LIST_TYPE)(FenceValue >> 56));
  Producer.WaitForFence(FenceValue);
}

}  // namespace re::engine::render