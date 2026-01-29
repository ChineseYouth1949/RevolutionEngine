#pragma once

#include "Engine/Base/PCH.h"

#define RE_DEFINE_TYPE_ID_FACTORY(Name)                \
  class RE_API Name {                                  \
   public:                                             \
    using ValueType = size_t;                          \
                                                       \
    template <typename T>                              \
    RE_FINLINE static ValueType Get() {                \
      static const ValueType id = sCount.fetch_add(1); \
      return id;                                       \
    }                                                  \
                                                       \
   private:                                            \
    static inline std::atomic<ValueType> sCount = 0;   \
  };

namespace re::engine::utility {
RE_DEFINE_TYPE_ID_FACTORY(TypeIdFactory)
}  // namespace re::engine::utility