#pragma once

#include "TypeId.h"

namespace re::engine::ecs {
enum class Access { Read, Write };

struct ResourceAccess {
  ResourceId resId;
  Access access;
};

struct ComponentAccess {
  ComponentId compId;
  Access access;
};

class RE_API Pass {
 public:
  virtual ~Pass() = default;

  virtual PassId GetPassId() const = 0;
  virtual const char* GetName() const { return "UnknownTask"; }

  virtual const std::vector<ResourceAccess>& GetResourceAccess() const = 0;
  virtual const std::vector<ComponentAccess>& GetComponentAccess() const = 0;
  virtual const std::vector<PassId>& GetBeforePass() const = 0;
  virtual const std::vector<PassId>& GetAfterPass() const = 0;

  virtual void Execute() = 0;

  tf::Task flowTask;
};

class Scheduler {
 public:
  void Compile(const std::vector<std::shared_ptr<Pass>>& passArray) {
    m_taskflow.clear();
    m_idToTask.clear();

    for (auto& pass : passArray) {
      pass->flowTask = m_taskflow.emplace([pass]() { pass->Execute(); }).name(pass->GetName());
      m_idToTask[pass->GetStageId()] = pass;
    }

    for (auto& [id, PassId] : m_idToTask) {
      for (auto beforeId : PassId->GetRunBefore()) {
        if (m_idToTask.count(beforeId))
          PassId->flowTask.precede(m_idToTask[beforeId]->flowTask);
      }
      for (auto afterId : PassId->GetRunAfter()) {
        if (m_idToTask.count(afterId))
          m_idToTask[afterId]->flowTask.precede(PassId->flowTask);
      }
    }

    BuildDataDependencies(tasks);
  }

  void Execute() { m_executor.run(m_taskflow).wait(); }

 private:
  void BuildDataDependencies(const std::vector<std::shared_ptr<Task>>& tasks) {
    // 记录每个资源最后一次的写入者，后续读者必须在写入者之后
    std::unordered_map<ResourceId, tf::Task> lastWriters;

    for (auto& PassId : tasks) {
      for (const auto& access : PassId->GetResourceAccess()) {
        if (access.access == Access::Write) {
          // 如果有之前的写入者，建立依赖
          if (lastWriters.count(access.resId)) {
            lastWriters[access.resId].precede(PassId->flowTask);
          }
          lastWriters[access.resId] = PassId->flowTask;
        } else {
          // 如果是读取，必须在最近一次写入之后
          if (lastWriters.count(access.resId)) {
            lastWriters[access.resId].precede(PassId->flowTask);
          }
        }
      }
      // ComponentAccess 的逻辑同理...
    }
  }

  tf::Executor m_executor;
  tf::Taskflow m_taskflow;
  std::unordered_map<PassId, std::shared_ptr<Task>> m_idToTask;
};

// template <typename T>
// class BaseTask : public Task {
//  public:
//   const vector<ResourceAccess>& GetResourceAccess() const override { return m_Resources; }
//   const vector<std::type_index>& GetRunBefore() const override { return m_Before; }
//   const vector<std::type_index>& GetRunAfter() const override { return m_After; }

//  protected:
//   template <typename Res>
//   void Read() {
//     m_Resources.push_back({TypeCounter::Next<Res>(), Access::Read});
//   }
//   template <typename Res>
//   void Write() {
//     m_Resources.push_back({TypeCounter::Next<Res>(), Access::Write});
//   }
//   template <typename TargetTask>
//   void Before() {
//     m_Before.push_back(typeid(TargetTask));
//   }
//   template <typename TargetTask>
//   void After() {
//     m_After.push_back(typeid(TargetTask));
//   }

//  private:
//   vector<ResourceAccess> m_Resources;
//   vector<std::type_index> m_Before;
//   vector<std::type_index> m_After;
// };

}  // namespace re::engine::ecs