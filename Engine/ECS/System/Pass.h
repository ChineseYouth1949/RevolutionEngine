#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
using PassId = uint32_t;
class PassIdFactory {
 public:
  static inline const PassId Null = 0;

  static PassId CreateId();

  template <typename T>
  static PassId GetClassId();
};

using PassMutexId = uint32_t;
class PassMutexIdFactory {
 public:
  template <typename T>
  static PassMutexId GetClassId();
};

class RE_API Pass {
 public:
  Pass(PassId id = PassIdFactory::Null, const string& name = "") : m_id(id), m_Name(name) {}
  virtual ~Pass() = default;

  RE_FINLINE PassId GetId() const { return m_id; }
  RE_FINLINE const string& GetName() const { return m_Name; }

  RE_FINLINE void AddPredecessor(PassId id) { return m_Predecessors.push_back(id); }
  RE_FINLINE const vector<PassId>& GetPredecessors() const { return m_Predecessors; }

  RE_FINLINE void AddMutexId(PassMutexId id) { return m_MutexIds.push_back(id); }
  RE_FINLINE const vector<PassMutex>& GetMutexIds() const { return m_MutexIds; }

  template <typename T>
  void SetIdToType();

  template <typename T>
  void SetPredecessorType();

  template <typename T>
  void SetMutexType();

  virtual void Execute() = 0;

 protected:
  PassId m_id;
  string m_Name;

  vector<PassId> m_Predecessors;
  vector<PassMutexId> m_MutexIds;
};

}  // namespace re::engine::ecs

#include "Pass.inl"