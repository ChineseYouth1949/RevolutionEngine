#pragma once

#include "Base/Export.h"

namespace RE::Core {

struct RE_API Vertex3D {
  Vertex3D() : x(0), y(0), z(0) {}

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