#pragma once

#include <memory>
#include <mutex>

#include <taskflow/taskflow.hpp>

namespace RE::Core {

class TaskSystem {
 public:
  static TaskSystem& Instance();

  void SetThreadNum(size_t num);

  template <typename F, typename... Args>
  auto Submit(F&& f, Args&&... args) {
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    return m_executor->async(std::move(task));
  }

  void WaitAllTask();

  size_t GetThreadNum() const;
  size_t GetWorkNum() const;

 private:
  TaskSystem();
  ~TaskSystem();

  TaskSystem(const TaskSystem&) = delete;
  TaskSystem& operator=(const TaskSystem&) = delete;

  std::unique_ptr<tf::Executor> m_executor;
  mutable std::mutex m_mutex;
};

}  // namespace RE::Core