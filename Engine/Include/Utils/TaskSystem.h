#pragma once

#include "Base/Base.h"
#include "Memory/Memory.h"

#include <shared_mutex>

#include <taskflow/taskflow.hpp>

namespace RE::Core {

class RE_API TaskSystem {
 public:
  static TaskSystem& Instance();

  void SetThreadNum(size_t num);

  template <typename F, typename... Args>
  auto Submit(F&& f, Args&&... args) {
    std::shared_lock lock(m_mutex);
    return m_executor->async([f = std::forward<F>(f), ... args = std::forward<Args>(args)] { return std::invoke(f, args...); });
  }

  void WaitAllTask();

  size_t GetThreadNum() const;
  size_t GetWorkNum() const;

 private:
  TaskSystem();
  ~TaskSystem();

  TaskSystem(TaskSystem&&) noexcept = default;
  TaskSystem& operator=(TaskSystem&&) noexcept = default;

  TaskSystem(const TaskSystem&) = delete;
  TaskSystem& operator=(const TaskSystem&) = delete;

  tf::Executor* m_executor;
  mutable std::shared_mutex m_mutex;
};

}  // namespace RE::Core