#pragma once

#include "Base/CompileDefine.h"
#include "Base/FlagType.h"

#include <string>
#include <vector>

namespace RE::Engine {

class Scene;

enum struct SceneLoadOption : int {
  None = 0,
  UseDirect12 = 1 << 0,
};

struct SceneLoadError {
  std::string filename;
  std::vector<std::string> infos;
};

class RE_API ISceneImporter {
 public:
  ISceneImporter() = default;
  virtual ~ISceneImporter() = default;

  ISceneImporter(ISceneImporter&&) noexcept = default;
  ISceneImporter& operator=(ISceneImporter&&) noexcept = default;

  ISceneImporter(const ISceneImporter&) = delete;
  ISceneImporter& operator=(const ISceneImporter&) = delete;

  virtual bool LoadScene(std::string fileName, Flag64 flags = 0) = 0;

  virtual int GetSceneNum() const = 0;
  virtual Scene* GetSceneIndex(int index, bool remove = false) = 0;
  virtual std::vector<Scene*> GetAllScene(bool remove = false) = 0;

  virtual const std::vector<SceneLoadError>& GetLoadErrors() const = 0;
  virtual void ClearLoadErrors() = 0;
};

}  // namespace RE::Engine