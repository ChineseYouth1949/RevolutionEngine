#include "Resource/ResourceManager.h"

#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure
#include <assimp/Importer.hpp>   // C++ importer interface

#include <vector>

#include "Resource/Model.h"

namespace RE::Core {

struct ResourceManager::Impl {
  std::vector<Assimp::Importer> assimp_importers;
};

ResourceManager::ResourceManager() {
  m_impl = std::make_shared<Impl>();
}

ResourceManager& ResourceManager::Instance() {
  static ResourceManager instance;
  return instance;
}

ResourceID ResourceManager::LoadModel() {
  return ResourceID();
}

Model& ResourceManager::GetModel(ResourceID id) {
  return Model();
}
void ResourceManager::FreeModel(ResourceID id) {}

}  // namespace RE::Core