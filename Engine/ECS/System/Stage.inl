#pragma once

#include "Stage.h"

namespace re::engine::ecs {
RE_FINLINE std::atomic<uint32_t>& GetStageClassCounter() {
  static std::atomic<uint32_t> counter{1};
  return counter;
}

template <typename T>
StageId StageIdFactory::GetClassId() {
  static const uint32_t id = GetStageClassCounter().fetch_add(1, std::memory_order_relaxed);
  return id;
}

StageId StageIdFactory::CreateId() {
  static const uint32_t id = GetStageClassCounter().fetch_add(1, std::memory_order_relaxed);
  return id;
}

template <typename T>
void Stage::SetIdToType() {
  m_id = StageIdFactory::GetClassId<T>();
}

template <typename T>
void Stage::SetPredecessorType() {
  StageId typeId = StageIdFactory::GetClassId<T>();
  AddPredecessor(typeId);
}

template <typename T>
void Stage::SetSuccessorType() {
  StageId typeId = StageIdFactory::GetClassId<T>();
  AddSuccessor(typeId);
}

}  // namespace re::engine::ecs