#pragma once

namespace re::engine::ecs {
enum class Access { Read, Write };

struct ResourceAccess {
  size_t typeId;
  Access access;
};

class Task {
 public:
  virtual ~Task() = default;

  // 核心接口：声明该任务的需求
  virtual const vector<ResourceAccess>& GetResourceAccess() const = 0;
  virtual const vector<std::type_index>& GetRunBefore() const = 0;
  virtual const vector<std::type_index>& GetRunAfter() const = 0;

  // 执行逻辑
  virtual void Execute(const UpdateInfo& info) = 0;

  // 用于 Taskflow 的 Task 句柄存储
  tf::Task flowTask;
};

// 模板封装，方便用户快速定义 Task
template <typename T>
class BaseTask : public Task {
 public:
  const vector<ResourceAccess>& GetResourceAccess() const override { return m_Resources; }
  const vector<std::type_index>& GetRunBefore() const override { return m_Before; }
  const vector<std::type_index>& GetRunAfter() const override { return m_After; }

 protected:
  template <typename Res>
  void Read() {
    m_Resources.push_back({TypeCounter::Next<Res>(), Access::Read});
  }
  template <typename Res>
  void Write() {
    m_Resources.push_back({TypeCounter::Next<Res>(), Access::Write});
  }
  template <typename TargetTask>
  void Before() {
    m_Before.push_back(typeid(TargetTask));
  }
  template <typename TargetTask>
  void After() {
    m_After.push_back(typeid(TargetTask));
  }

 private:
  vector<ResourceAccess> m_Resources;
  vector<std::type_index> m_Before;
  vector<std::type_index> m_After;
};

}  // namespace re::engine::ecs