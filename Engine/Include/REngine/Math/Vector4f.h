#pragma once

#include "Base/Base.h"

namespace RE::Engine {

struct RE_DECLSPEC Vector4f {
  constexpr Vector4f() : x(0), y(0), z(0), w(0) {}
  constexpr Vector4f(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

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

}  // namespace RE::Engine