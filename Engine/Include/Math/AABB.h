#pragma once

#include "Base/Base.h"

#include "Vector2f.h"

namespace RE::Core {

struct RE_API AABB {
  AABB() : m_min(MaxVector2f()), m_max(NevMaxVector2f()) {}

  Vector2f m_min;
  Vector2f m_max;
};

}  // namespace RE::Core