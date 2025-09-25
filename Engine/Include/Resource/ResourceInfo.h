#pragma once

#include <inttypes.h>

namespace RE::Core {

enum struct ResourceType {
  None = 0,
  Shader,
  Model,
  Num,
};

using ResourceID = uint64_t;

}  // namespace RE::Core