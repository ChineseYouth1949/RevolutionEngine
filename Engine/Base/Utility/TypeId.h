#pragma once

#include "Engine/Base/PCH.h"

namespace re::engine::utility {
class RE_API TypeId {
 public:
  template <typename T>
  RE_FINLINE static size_t Get() {
    static const size_t id = Next();
    return id;
  }

 private:
  static size_t Next();
};
}  // namespace re::engine::utility