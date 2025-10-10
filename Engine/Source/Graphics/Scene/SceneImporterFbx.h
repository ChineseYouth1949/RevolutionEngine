#pragma once

#include "Graphics/Scene/ISceneImporter.h"

#include <fbxsdk.h>

namespace RE::Core {

class RE_API SceneImporterFBX : public ISceneImporter {
 public:
  SceneImporterFBX();
  virtual ~SceneImporterFBX();

  SceneImporterFBX(SceneImporterFBX&&) noexcept;
  SceneImporterFBX& operator=(SceneImporterFBX&&) noexcept;

  SceneImporterFBX(const SceneImporterFBX&) = delete;
  SceneImporterFBX& operator=(const SceneImporterFBX&) = delete;

  bool LoadScene(std::string fileName, Flag64 flags);
  std::vector<std::string> GetErrorString() const;

  int GetSceneNum() const;
  Scene* GetSceneIndex(int index);
  std::vector<Scene*> GetAllScene();

 protected:
  static void InitFbxSdk();
  void Destroy();

 private:
  FbxManager* m_fbxSdkManager;
  std::vector<Scene*> m_scenes;
  std::vector<std::string> m_errorStrings;
};

}  // namespace RE::Core