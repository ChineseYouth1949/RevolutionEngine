#pragma once

#include "Base/CompileDefine.h"
#include "Base/FlagType.h"

namespace RE::Core {

class Scene;

enum struct SceneImporterOption {
  None = 0,
};

class RE_API ISceneImporter {
 public:
  ISceneImporter() = default;
  virtual ~ISceneImporter() = default;

  ISceneImporter(ISceneImporter&&) noexcept = default;
  ISceneImporter& operator=(ISceneImporter&&) noexcept = default;
  ISceneImporter(const ISceneImporter&) = delete;
  ISceneImporter& operator=(const ISceneImporter&) = delete;

  virtual bool LoadScene(std::string fileName, Flag64 flags) = 0;
  virtual std::string LoadErrorString() const = 0;

  virtual std::vector<Scene*> GetAllScene() = 0;
  virtual Scene* GetLastScene() = 0;

 private:
  Assimp::Importer m_assimpImporter;
  std::vector<GUniquePtr<Scene>> m_scenes;

  bool m_lastSuccess;
  std::string m_lastErrorString;
};

}  // namespace RE::Core
