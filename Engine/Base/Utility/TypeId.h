#pragma once

#include "Engine/Base/PCH.h"

#define RE_DEFINE_ID_FACTORY(_Name, _MaxId)                \
  class RE_API _Name {                                     \
   public:                                                 \
    using ValueType = size_t;                              \
    static inline const ValueType Max = _MaxId;            \
    static inline const ValueType Null = 0;                \
                                                           \
    RE_FINLINE static ValueType CreateId() {               \
      ValueType id = sCount.fetch_add(1);                  \
      assert(id < Max && "ID allocation exceeded MaxId!"); \
      return id;                                           \
    }                                                      \
                                                           \
    template <typename T>                                  \
    RE_FINLINE static ValueType GetTypeConstId() {         \
      static const ValueType id = sCount.fetch_add(1);     \
      assert(id < Max && "ID allocation exceeded MaxId!"); \
      return id;                                           \
    }                                                      \
                                                           \
   private:                                                \
    static inline std::atomic<ValueType> sCount{1};        \
  };

namespace re::engine::utility {
RE_DEFINE_ID_FACTORY(IdFactory, SIZE_MAX)
}  // namespace re::engine::utility