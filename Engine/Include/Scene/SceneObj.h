#pragma once

#include "Base/Base.h"
#include "Math/Math.h"

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

namespace RE::Core {

struct RE_API Scene {

  void Destroy();

  std::vector<std::string> mAnimStackNames;
  std::vector<Texture*> mTextures;
  std::vector<Material*> mMaterials;

  std::vector<Mesh*> mMeshs;
};

}  // namespace RE::Core