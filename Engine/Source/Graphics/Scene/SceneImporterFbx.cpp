#include "SceneImporterFbx.h"

#include "Base/Assert.h"
#include "Memory/Memory.h"

#include "Graphics/Scene/Scene.h"

namespace RE::Core {

std::atomic<int> SceneImporterFBX::sImporterIndex = 0;
std::atomic<int> SceneImporterFBX::sSceneIndex = 0;

void SceneImporterFBX::InitFbxSdk() {
  static bool sInited = false;

  if (!sInited) {
    FbxSetMallocHandler(GMalloc);
    FbxSetCallocHandler(GCalloc);
    FbxSetReallocHandler(GRealloc);
    FbxSetFreeHandler(GFree);

    sInited = true;
  }
}

SceneImporterFBX::SceneImporterFBX() {
  InitFbxSdk();

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

  if (m_fbxImporter->Initialize(fileName.c_str(), fileFormat) == true) {
    std::string sceneName = "Scene-" + std::to_string(sSceneIndex++);
    FbxScene* fbxScene = FbxScene::Create(m_fbxSdkManager, sceneName.c_str());

    FbxStatus status;
    FbxArray<FbxString*> details;
    FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(fbxScene), &status, &details);
    bool notify = (!sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData) && details.GetCount() > 0) ||
                  (m_fbxImporter->GetStatus().GetCode() != FbxStatus::eSuccess);

    if (notify) {
      std::string errorString;

      if (details.GetCount()) {
        errorString = "Scene integrity verification failed with the following errors:\n";

        for (int i = 0; i < details.GetCount(); i++) {
          errorString += std::string(details[i]->Buffer());
          errorString += "\n";
        }
      }

      if (m_fbxImporter->GetStatus().GetCode() != FbxStatus::eSuccess) {
        errorString += "WARNING:\n";
        errorString += "   The importer was able to read the file but with errors.\n";
        errorString += "   Loaded scene may be incomplete.\n\n";
        errorString += "   Last error message:'%s'\n";
        errorString += std::string(m_fbxImporter->GetStatus().GetErrorString());
      }

      SceneLoadError error;
      error.filename = fileName;
      error.info = std::move(errorString);

      m_loadErrors.push_back(std::move(error));
      return false;
    }

    FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
    FbxAxisSystem targetAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);

    bool useLeftHanded = CheckFlag(flags, long(SceneLoadOption::UseDirect12));
    if (useLeftHanded) {
      targetAxisSystem = FbxAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded);
    }

    if (sceneAxisSystem != targetAxisSystem) {
      targetAxisSystem.ConvertScene(fbxScene);
    }

    return true;
  } else {
    std::string errorString = "Unable to open file ";
    errorString += fileName;
    errorString += "\nError reported: ";
    errorString += std::string(m_fbxImporter->GetStatus().GetErrorString());

    SceneLoadError error;
    error.filename = fileName;
    error.info = std::move(errorString);

    m_loadErrors.push_back(std::move(error));
    return false;
  }
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