#pragma once

#include "Pass.h"

namespace re::engine::ecs {
RE_FINLINE std::atomic<uint32_t>& GetPassClassCounter() {
  static std::atomic<uint32_t> counter{1};
  return counter;
}

template <typename T>
PassId PassIdFactory::GetClassId() {
  static const uint32_t id = GetPassClassCounter().fetch_add(1, std::memory_order_relaxed);
  return id;
}

PassId PassIdFactory::CreateId() {
  static const uint32_t id = GetPassClassCounter().fetch_add(1, std::memory_order_relaxed);
  return id;
}

template <typename T>
PassMutexId PassMutexIdFactory::GetClassId() {
  static const uint32_t id = GetPassClassCounter().fetch_add(1, std::memory_order_relaxed);
  return id;
}

template <typename T>
void Pass::SetIdToType() {
  m_id = PassIdFactory::GetClassId<T>();
}

template <typename T>
void Pass::SetPredecessorType() {
  PassId typeId = PassIdFactory::GetClassId<T>();
  AddPredecessor(typeId);
}

template <typename T>
void Pass::SetMutexType() {
  PassMutexId mutexId = PassMutexIdFactory::GetClassId<T>();
  AddMutexId(mutexId);
}

}  // namespace re::engine::ecs