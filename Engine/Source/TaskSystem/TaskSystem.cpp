#include "TaskSystem/TaskSystem.h"

namespace RE::Core {

TaskSystem& TaskSystem::Instance() {
  static TaskSystem instance;
  return instance;
}

TaskSystem::TaskSystem() {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_executor = std::make_unique<tf::Executor>();
}

TaskSystem::~TaskSystem() {
  WaitAllTask();
}

void TaskSystem::SetThreadNum(size_t num) {
  WaitAllTask();
  std::lock_guard<std::mutex> lock(m_mutex);
  m_executor = std::make_unique<tf::Executor>(num);
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