#include "Utils/TaskSystem.h"
#include "Memory/Memory.h"

namespace RE::Core {

TaskSystem& TaskSystem::Instance() {
  static TaskSystem instance;
  return instance;
}

TaskSystem::TaskSystem() : m_executor(MemoryAllocator::DefaultNew<tf::Executor>()) {
  std::lock_guard<std::mutex> lock(m_mutex);
}

TaskSystem::~TaskSystem() {
  WaitAllTask();
  MemoryAllocator::DefaultDelete(m_executor);
  m_executor = nullptr;
}

void TaskSystem::SetThreadNum(size_t num) {
  WaitAllTask();
  std::lock_guard<std::mutex> lock(m_mutex);

  MemoryAllocator::DefaultDelete(m_executor);
  m_executor = nullptr;
  m_executor = MemoryAllocator::DefaultNew<tf::Executor>();
}

void TaskSystem::WaitAllTask() {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_executor->wait_for_all();
}

size_t TaskSystem::GetThreadNum() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_executor->num_workers();
}
size_t TaskSystem::GetWorkNum() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_executor->num_topologies();
}

}  // namespace RE::Core