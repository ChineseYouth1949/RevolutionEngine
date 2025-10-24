#pragma once

#include "Base/CompileDefine.h"
#include "Object/RObject.h"

#include "Vector2f.h"

namespace RE::Engine {

struct RE_DECLSPEC AABB : public RObject {
  AABB() : m_min(MaxVector2f()), m_max(NevMaxVector2f()) {}

  Vector2f m_min;
  Vector2f m_max;
};

}  // namespace RE::Engine