#include "SceneImporterFbx.h"

#include "Base/Assert.h"
#include "Memory/Memory.h"

#include "Scene/SceneObj.h"

#include "Auxiliary.h"

namespace RE::Core {

std::atomic<int> SceneImporterFBX::sImporterIndex = 0;
std::atomic<int> SceneImporterFBX::sSceneIndex = 0;

SceneImporterFBX::SceneImporterFBX() {
  FBXSdkInit();

  m_fbxSdkManager = FbxManager::Create();
  if (!m_fbxSdkManager) {
    AssertInfo(false, "Error: Unable to create FBX Manager!")
  }

  std::string importerName = "Importer-" + std::to_string(sImporterIndex++);
  m_fbxImporter = FbxImporter::Create(m_fbxSdkManager, importerName.c_str());

  FbxIOSettings* fbx_ios = FbxIOSettings::Create(m_fbxSdkManager, IOSROOT);
  m_fbxSdkManager->SetIOSettings(fbx_ios);
}

void SceneImporterFBX::Destroy() {
  if (m_fbxSdkManager) {
    m_fbxSdkManager->Destroy();
    m_fbxSdkManager = nullptr;
    m_fbxImporter = nullptr;
  }

  for (auto scene : m_scenes) {
    if (scene) {
      scene->Destroy();
      GFree(scene);
    }
  }
  m_scenes.clear();
}

SceneImporterFBX::~SceneImporterFBX() {
  Destroy();
}

SceneImporterFBX::SceneImporterFBX(SceneImporterFBX&& other) noexcept {
  m_fbxSdkManager = other.m_fbxSdkManager;
  other.m_fbxSdkManager = nullptr;

  m_scenes = std::move(other.m_scenes);
}
SceneImporterFBX& SceneImporterFBX::operator=(SceneImporterFBX&& other) noexcept {
  if (this != &other) {
    Destroy();

    m_fbxSdkManager = other.m_fbxSdkManager;
    other.m_fbxSdkManager = nullptr;

    m_scenes = std::move(other.m_scenes);
  }
  return *this;
}

bool SceneImporterFBX::LoadScene(std::string fileName, Flag64 flags) {
  AssertInfo(m_fbxSdkManager != nullptr, "m_fbxSdkManager is nullptr");

  int fileFormat = -1;
  if (m_fbxSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(fileName.c_str(), fileFormat)) {
    fileFormat = m_fbxSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
  }

  bool success = m_fbxImporter->Initialize(fileName.c_str(), fileFormat);

  if (!success) {
    std::string errorString = "Unable to open file ";
    errorString += fileName;
    errorString += "\nError reported: ";
    errorString += std::string(m_fbxImporter->GetStatus().GetErrorString());

    SceneLoadError error;
    error.filename = fileName;
    error.infos.push_back(std::move(errorString));

    m_loadErrors.push_back(std::move(error));
    return false;
  }

  std::string sceneName = "Scene-" + std::to_string(sSceneIndex++);
  FbxScene* fbxScene = FbxScene::Create(m_fbxSdkManager, sceneName.c_str());

  success = m_fbxImporter->Import(fbxScene);
  if (!success) {
    std::string errorString = "Unable to open file ";
    errorString += fileName;
    errorString += "\nError reported: ";
    errorString += std::string(m_fbxImporter->GetStatus().GetErrorString());

    SceneLoadError error;
    error.filename = fileName;
    error.infos.push_back(std::move(errorString));

    m_loadErrors.push_back(std::move(error));
    return false;
  }

  Scene* importScene = nullptr;
  std::vector<std::string> errorInfos;
  success = FBXSceneTransform(m_fbxSdkManager, m_fbxImporter, fbxScene, flags, importScene, errorInfos);

  if (!success) {
    SceneLoadError error;
    error.filename = fileName;
    error.infos = std::move(errorInfos);

    m_loadErrors.push_back(std::move(error));
    return false;
  }

  m_scenes.push_back(importScene);
  return true;
}

int SceneImporterFBX::GetSceneNum() const {
  return m_scenes.size();
}

Scene* SceneImporterFBX::GetSceneIndex(int index, bool remove) {
  Scene* res_scene = m_scenes[index];

  if (remove) {
    m_scenes.erase(m_scenes.begin() + index);
  }

  return res_scene;
}

std::vector<Scene*> SceneImporterFBX::GetAllScene(bool remove) {
  if (remove) {
    return std::move(m_scenes);
  } else {
    return m_scenes;
  }
}

const std::vector<SceneLoadError>& SceneImporterFBX::GetLoadErrors() const {
  return m_loadErrors;
}

void SceneImporterFBX::ClearLoadErrors() {
  m_loadErrors.clear();
}

}  // namespace RE::Core