#pragma once

#include "Engine/Base/PCH.h"

#define RE_DEFINE_ID_FACTORY(Name, MaxId)                    \
  class RE_API Name {                                        \
   public:                                                   \
    using ValueType = size_t;                                \
    static inline const ValueType sMaxId = MaxId;            \
                                                             \
    RE_FINLINE static ValueType CreateId() {                 \
      ValueType id = sCount.fetch_add(1);                    \
      assert(id < MaxId && "ID allocation exceeded MaxId!"); \
      return id;                                             \
    }                                                        \
                                                             \
    template <typename T>                                    \
    RE_FINLINE static ValueType GetTypeConstId() {           \
      static const ValueType id = sCount.fetch_add(1);       \
      assert(id < MaxId && "ID allocation exceeded MaxId!"); \
      return id;                                             \
    }                                                        \
                                                             \
   private:                                                  \
    static inline std::atomic<ValueType> sCount{0};          \
  };

namespace re::engine::utility {
RE_DEFINE_ID_FACTORY(IdFactory, SIZE_MAX)
}  // namespace re::engine::utility