#include "Resource/Scene/SceneImporter.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <vector>
#include <future>
#include <mutex>

#include "TaskSystem/TaskSystem.h"

namespace RE::Core {

struct SceneImporter::Impl {
  // info data
  bool useable = true;
  std::string errorString;
  std::mutex infoMutex;

  // task data
  Assimp::Importer assimpImporter;
  std::shared_ptr<Scene> scene;
  std::future<void> taskFuture;
  std::mutex taskMutex;
};

SceneImporter::SceneImporter() {
  m_impl = new Impl();
}

SceneImporter::~SceneImporter() {
  WaitAsyncComplete();
  delete m_impl;
}

bool SceneImporter::IsUseable() const {
  std::lock_guard<std::mutex> lock(m_impl->infoMutex);
  return m_impl->useable;
}

bool SceneImporter::LoadScene(const std::string& file, unsigned int flags) {
  std::lock_guard<std::mutex> lock(m_impl->infoMutex);

  if (!m_impl->useable) {
    return false;
  }

  m_impl->useable = false;
  LoadSceneToImporter(this, file, flags, false);

  return true;
}

bool SceneImporter::LoadSceneAsync(const std::string& file, unsigned int flags) {
  std::lock_guard<std::mutex> lock(m_impl->infoMutex);

  if (!m_impl->useable) {
    return false;
  }

  auto& taskSystemInstance = TaskSystem::Instance();

  m_impl->useable = false;
  m_impl->taskFuture = taskSystemInstance.Submit(LoadSceneToImporter, this, file, flags, true);

  return true;
}

std::shared_ptr<Scene> SceneImporter::GetScene() {
  std::lock_guard<std::mutex> lock(m_impl->taskMutex);
  if (m_impl->taskFuture.valid()) {
    m_impl->taskFuture.wait();
  }
  return m_impl->scene;
}

void SceneImporter::WaitAsyncComplete() {
  if (m_impl->taskFuture.valid()) {
    m_impl->taskFuture.wait();
  }
}

bool SceneImporter::LoadSuccess() {
  return GetScene() != nullptr;
}

std::string SceneImporter::ErrorString() const {
  std::lock_guard<std::mutex> lock(m_impl->infoMutex);
  return m_impl->errorString;
}

void SceneImporter::LoadSceneToImporter(SceneImporter* pSceneImporter, const std::string& file, unsigned int flags, bool async) {
  if (async) {
    pSceneImporter->m_impl->taskMutex.lock();
  }

  unsigned int constFlag = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;

  const aiScene* scene = pSceneImporter->m_impl->assimpImporter.ReadFile(file, constFlag);
  std::shared_ptr<Scene> result_scene = std::make_shared<Scene>();

  if (async) {
    pSceneImporter->m_impl->infoMutex.lock();
  }

  if (scene == nullptr) {
    pSceneImporter->m_impl->errorString = pSceneImporter->m_impl->assimpImporter.GetErrorString();
  }

  pSceneImporter->m_impl->useable = true;
  pSceneImporter->m_impl->scene = result_scene;

  if (async) {
    pSceneImporter->m_impl->taskMutex.unlock();
    pSceneImporter->m_impl->infoMutex.unlock();
  }
}

}  // namespace RE::Core