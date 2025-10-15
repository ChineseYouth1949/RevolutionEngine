#pragma once

#include "ISceneImporter.h"

namespace RE::Core {

enum struct SceneImporterType { None = 0, FBX_SDK, Assimp };

class SceneImporterFactory {
 public:
  static ISceneImporter* Create();
};

}  // namespace RE::Core