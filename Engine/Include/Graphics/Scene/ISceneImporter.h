#pragma once

#include "Base/CompileDefine.h"
#include "Base/FlagType.h"

#include <string>
#include <vector>

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
  virtual std::vector<std::string> GetErrorString() const = 0;

  virtual int GetSceneNum() const = 0;
  virtual Scene* GetSceneIndex(int index) = 0;
  virtual std::vector<Scene*> GetAllScene() = 0;
};

}  // namespace RE::Core
