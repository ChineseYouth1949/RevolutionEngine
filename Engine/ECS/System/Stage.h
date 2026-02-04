#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
using StageId = uint32_t;
using StageMutexTag = uint32_t;

class StageIdFactory {
 public:
  static inline const StageId Null = 0;

  static StageId CreateId();

  template <typename T>
  static StageId GetClassId();
};

class RE_API Stage {
 public:
  Stage(StageId id = StageIdFactory::Null, const string& name = "") : m_id(id), m_Name(name) {}
  virtual ~Stage() = default;

  RE_FINLINE StageId GetId() const { return m_id; }
  RE_FINLINE const string& GetName() const { return m_Name; }

  RE_FINLINE const vector<StageId>& GetPredecessors() const { return m_Predecessors; }
  RE_FINLINE const vector<StageId>& GetSuccessors() const { return m_Successors; }
  RE_FINLINE const vector<StageMutexTag>& GetMutexTag() const { return m_MutexTag; }

  RE_FINLINE void AddPredecessor(StageId id) { return m_Predecessors.push_back(id); }
  RE_FINLINE void AddSuccessor(StageId id) { return m_Successors.push_back(id); }
  RE_FINLINE void AddMutexTag(StageMutexTag tag) { return m_MutexTag.push_back(tag); }

  template <typename T>
  void SetIdToType();

  template <typename T>
  void SetPredecessorType();

  template <typename T>
  void SetSuccessorType();

  virtual void Execute() = 0;

 protected:
  StageId m_id;
  string m_Name;

  vector<StageId> m_Predecessors;
  vector<StageId> m_Successors;
  vector<StageMutexTag> m_MutexTag;
};

}  // namespace re::engine::ecs

#include "Stage.inl"