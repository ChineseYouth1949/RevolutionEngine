#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
using TaskId = uint32_t;

template <typename Type>
RE_FINLINE static const utility::Descriptor& GetTaskDescriptor() {
  return utility::DescriptorFactory::GetAt<Task, Type>();
}

class RE_API Task {
 public:
  Task(TaskId id, const string& name = "") : m_id(id), m_Name(name) {}
  virtual ~Task() = default;

  RE_FINLINE TaskId Id() const { return m_id; }
  RE_FINLINE const string& Name() const { return m_Name; }

  RE_FINLINE const vector<TaskId>& Predecessors() const { return m_Predecessors; }
  RE_FINLINE const vector<TaskId>& Successors() const { return m_Successors; }

  template <typename TaskType>
  Task& DependsOn() {
    TaskId targetId = GetTaskDescriptor<TaskType>().id;
    RE_ASSERT(targetId != m_id && "Task cannot depend on itself!");
    if (std::find(m_Predecessors.begin(), m_Predecessors.end(), targetId) == m_Predecessors.end()) {
      m_Predecessors.push_back(targetId);
    }
    return *this;
  }

  template <typename TaskType>
  Task& RequiredBy() {
    TaskId targetId = GetTaskDescriptor<TaskType>().id;
    RE_ASSERT(targetId != m_id && "Task cannot depend on itself!");
    if (std::find(m_Successors.begin(), m_Successors.end(), targetId) == m_Successors.end()) {
      m_Successors.push_back(targetId);
    }
    return *this;
  }

  virtual void Execute() = 0;

 protected:
  TaskId m_id;
  string m_Name;

  vector<TaskId> m_Predecessors;
  vector<TaskId> m_Successors;
};

}  // namespace re::engine::ecs