#pragma once

#include "Base/Base.h"

namespace RE::Engine {

struct RE_DECLSPEC Vector3f {
  constexpr Vector3f() : x(0), y(0), z(0) {}
  constexpr Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

  union {
    struct {
      float x;
      float y;
      float z;
    };
    float v[3];
  };
};

}  // namespace RE::Engine