#pragma once

#include "Graphics/Scene/ISceneImporter.h"

#include <atomic>

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

  bool LoadScene(std::string fileName, Flag64 flags = 0) override;

  int GetSceneNum() const override;
  Scene* GetSceneIndex(int index, bool remove = true) override;
  std::vector<Scene*> GetAllScene(bool remove = true) override;

  const std::vector<SceneLoadError>& GetLoadErrors() const override;
  void ClearLoadErrors() override;

 protected:
  static void InitFbxSdk();
  void Destroy();

 private:
  FbxManager* m_fbxSdkManager;
  FbxImporter* m_fbxImporter;

  static std::atomic<int> sImporterIndex;
  static std::atomic<int> sSceneIndex;

  std::vector<Scene*> m_scenes;
  std::vector<SceneLoadError> m_loadErrors;
};

}  // namespace RE::Core