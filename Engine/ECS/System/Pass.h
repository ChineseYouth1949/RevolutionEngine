#pragma once

#include "PassInfo.h"

namespace re::engine::ecs {
class RE_API Pass {
 public:
  Pass() = default;
  Pass(PassId id, const string& name) : m_id(id), m_Name(name) {}
  virtual ~Pass() = default;

  RE_FINLINE PassId GetId() const { return m_id; }
  RE_FINLINE const string& GetName() const { return m_Name; }

  RE_FINLINE void AddPredecessor(PassId id) { return m_Predecessors.push_back(id); }
  RE_FINLINE const vector<PassId>& GetPredecessors() const { return m_Predecessors; }

  RE_FINLINE void AddMutex(PassMutex mutex) { return m_Mutexs.push_back(mutex); }
  RE_FINLINE const vector<PassMutex>& GetMutexs() const { return m_Mutexs; }

  virtual void Execute() = 0;

 protected:
  PassId m_id;
  string m_Name;

  vector<PassId> m_Predecessors;
  vector<PassMutex> m_Mutexs;
};

}  // namespace re::engine::ecs