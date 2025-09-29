#pragma once

#include <limits>

#include "Base/Base.h"

namespace RE::Core {

struct RE_API Vector2f {
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

}  // namespace RE::Core