#pragma once

#include "ISceneImporter.h"

namespace RE::Engine {

enum struct SceneImporterType { None = 0, FBX_SDK, Assimp };

class SceneImporterFactory {
 public:
  static ISceneImporter* Create();
};

}  // namespace RE::Engine