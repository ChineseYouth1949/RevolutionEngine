#pragma once

#include "Base/Base.h"
#include "Math/Math.h"

#include "Mesh.h"
#include "Texture.h"

namespace RE::Core {

struct RE_API Scene {

  void Destroy();

  std::vector<std::string> mAnimStackNames;
  std::vector<Texture*> m_textures;
  std::vector<Mesh*> m_meshs;
};

}  // namespace RE::Core