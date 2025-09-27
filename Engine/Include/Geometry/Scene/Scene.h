#pragma once

#include "Base/Base.h"
#include "Math/Math.h"

#include "Mesh.h"

namespace RE::Core {

struct RE_API Scene {

  std::vector<Mesh> m_meshs;
};

}  // namespace RE::Core