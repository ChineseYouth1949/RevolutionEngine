#include "SceneImporterFbx.h"

#include "Graphics/Scene/Scene.h"

#include "Memory/Memory.h"

namespace RE::Core {

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

  FbxIOSettings* fbx_ios = FbxIOSettings::Create(m_fbxSdkManager, IOSROOT);
  m_fbxSdkManager->SetIOSettings(fbx_ios);
}

void SceneImporterFBX::Destroy() {
  if (m_fbxSdkManager) {
    m_fbxSdkManager->Destroy();
    m_fbxSdkManager = nullptr;
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

  return false;
}

std::vector<std::string> SceneImporterFBX::GetErrorString() const {
  return m_errorStrings;
}

int SceneImporterFBX::GetSceneNum() const {
  return m_scenes.size();
}

Scene* SceneImporterFBX::GetSceneIndex(int index) {
  Scene* res_scene = m_scenes[index];
  m_scenes.erase(m_scenes.begin() + index);
  return res_scene;
}

std::vector<Scene*> SceneImporterFBX::GetAllScene() {
  return std::move(m_scenes);
}

}  // namespace RE::Core