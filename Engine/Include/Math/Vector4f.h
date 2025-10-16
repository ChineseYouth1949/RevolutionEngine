#pragma once

#include "Base/Base.h"

namespace RE::Core {

struct RE_API Vector4f {
  constexpr Vector4f() : x(0), y(0), z(0), w(0) {}

  union {
    struct {
      float x;
      float y;
      float z;
      float w;
    };
    struct {
      float r;
      float g;
      float b;
      float a;
    };
    float v[3];
  };
};

}  // namespace RE::Core