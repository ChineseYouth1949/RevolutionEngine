#include "Graphics/Scene/SceneImporter.h"

#include <vector>
#include <future>
#include <mutex>

#include "Utils/TaskSystem.h"
#include "Memory/Memory.h"

namespace RE::Core {

SceneImporter::SceneImporter() : m_lastSuccess(false) {}

SceneImporter::~SceneImporter() {}

bool SceneImporter::LoadScene(std::string fileName, unsigned int flags) {
  const aiScene* scene = m_assimpImporter.ReadFile(fileName, flags);

  if (scene == nullptr) {
    m_lastSuccess = false;
    m_lastErrorString = m_assimpImporter.GetErrorString();
    return false;
  } else {
    m_lastSuccess = true;
    m_lastErrorString.clear();
  }

  GUniquePtr<Scene> newScene = GAllocateConstrUnique<Scene>();

  m_scenes.push_back(std::move(newScene));

  return true;
}

GUniquePtr<Scene> SceneImporter::GetLastScene() {
  if (m_scenes.empty()) {
    return nullptr;
  }

  auto lastScene = std::move(m_scenes.back());
  m_scenes.pop_back();

  return std::move(lastScene);
}

std::vector<GUniquePtr<Scene>> SceneImporter::GetAllScene() {
  return std::move(m_scenes);
}

bool SceneImporter::LastLoadSuccess() const {
  return m_lastSuccess;
}

std::string SceneImporter::LastLoadErrorString() const {
  return m_lastErrorString;
}

}  // namespace RE::Core