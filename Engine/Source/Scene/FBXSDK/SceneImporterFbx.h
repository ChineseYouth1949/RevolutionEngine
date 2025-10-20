#pragma once

#include <atomic>
#include <fbxsdk.h>

#include "Scene/ISceneImporter.h"

namespace RE::Engine {

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
  Scene* GetSceneIndex(int index, bool remove = false) override;
  std::vector<Scene*> GetAllScene(bool remove = false) override;

  const std::vector<SceneLoadError>& GetLoadErrors() const override;
  void ClearLoadErrors() override;

 protected:
  static std::atomic<int> sImporterIndex;
  static std::atomic<int> sSceneIndex;

  void Destroy();

 private:
  FbxManager* m_fbxSdkManager;
  FbxImporter* m_fbxImporter;

  std::vector<Scene*> m_scenes;
  std::vector<SceneLoadError> m_loadErrors;
};

}  // namespace RE::Engine