#include "Utils/TaskSystem.h"
#include "Memory/Memory.h"

namespace RE::Engine {

TaskSystem& TaskSystem::Instance() {
  static TaskSystem instance;
  return instance;
}

TaskSystem::TaskSystem() : m_executor(GAllocateConstructor<tf::Executor>()) {}

TaskSystem::~TaskSystem() {
  WaitAllTask();
  GDestry<tf::Executor>(m_executor);
}

void TaskSystem::SetThreadNum(size_t num) {
  WaitAllTask();
  std::unique_lock lock(m_mutex);

  GDestry<tf::Executor>(m_executor);
  m_executor = GAllocateConstructor<tf::Executor>(num);
}

void TaskSystem::WaitAllTask() {
  std::shared_lock lock(m_mutex);
  m_executor->wait_for_all();
}

size_t TaskSystem::GetThreadNum() const {
  std::shared_lock lock(m_mutex);
  return m_executor->num_workers();
}
size_t TaskSystem::GetWorkNum() const {
  std::shared_lock lock(m_mutex);
  return m_executor->num_topologies();
}

}  // namespace RE::Engine