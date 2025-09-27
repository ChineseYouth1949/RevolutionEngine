#pragma once

#include "Base/Base.h"

namespace RE::Core {

struct RE_API Vector3f {
  constexpr Vector3f() : x(0), y(0), z(0) {}

  union {
    struct {
      float x;
      float y;
      float z;
    };
    float v[3];
  };
};

}  // namespace RE::Core