#pragma once

#include "Base/Base.h"
#include "Memory/Memory.h"

#include "Scene.h"

#include <string>
#include <mutex>
#include <shared_mutex>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace RE::Core {

class RE_API SceneImporter {
 public:
  SceneImporter();
  ~SceneImporter();

  bool LoadScene(std::string fileName, unsigned int flags);

  GUniquePtr<Scene> GetLastScene();
  std::vector<GUniquePtr<Scene>> GetAllScene();

  bool LastLoadSuccess() const;
  std::string LastLoadErrorString() const;

 private:
  SceneImporter(SceneImporter&&) noexcept = default;
  SceneImporter& operator=(SceneImporter&&) noexcept = default;

  SceneImporter(const SceneImporter&) = delete;
  SceneImporter& operator=(const SceneImporter&) = delete;

  Assimp::Importer m_assimpImporter;
  std::vector<GUniquePtr<Scene>> m_scenes;

  bool m_lastSuccess;
  std::string m_lastErrorString;
};

}  // namespace RE::Core
