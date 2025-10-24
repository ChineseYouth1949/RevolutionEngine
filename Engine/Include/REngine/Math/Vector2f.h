#pragma once

#include <limits>

#include "Base/CompileDefine.h"
#include "Object/RObject.h"

namespace RE::Engine {

struct RE_DECLSPEC Vector2f : public RObject {
  constexpr Vector2f() noexcept : x(0), y(0) {}
  constexpr Vector2f(float _x, float _y) noexcept : x(_x), y(_y) {}

  union {
    struct {
      float x;
      float y;
    };
    float v[2];
  };
};

RE_INLINE constexpr Vector2f MaxVector2f() {
  constexpr const float floatMaxValue = std::numeric_limits<float>::max();
  return {floatMaxValue, floatMaxValue};
}
RE_INLINE constexpr Vector2f NevMaxVector2f() {
  constexpr const float floatMaxValue = std::numeric_limits<float>::max();
  return {-floatMaxValue, -floatMaxValue};
}

}  // namespace RE::Engine