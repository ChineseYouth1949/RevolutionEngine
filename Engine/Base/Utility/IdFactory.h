#pragma once

#include "Engine/Base/PCH.h"

namespace re::engine::utility {

class RE_API IdFactory {
 public:
  using Id = uint32_t;

  RE_FINLINE static Id CreateId() { return sIdCount.fetch_add(1, std::memory_order_relaxed); }

  template <typename Class>
  RE_FINLINE static Id CreateClassId() {
    static std::atomic<uint32_t> classCounter{0};
    return classCounter.fetch_add(1, std::memory_order_relaxed);
  }

  template <typename Class, typename Type>
  RE_FINLINE static Id GetClassTypeId() {
    static const Id id = CreateClassId<Class>();
    return id;
  }

 private:
  static inline std::atomic<uint32_t> sIdCount{0};
};
}  // namespace re::engine::utility