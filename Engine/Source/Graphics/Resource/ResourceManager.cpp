// #include "Resource/ResourceManager.h"

// #include <assimp/postprocess.h>  // Post processing flags
// #include <assimp/scene.h>        // Output data structure
// #include <assimp/Importer.hpp>   // C++ importer interface

// #include <vector>
// #include <future>
// #include <mutex>

// #include "Resource/Model.h"

// namespace RE::Core {

// struct ModelImporter {
//  public:
//   ModelImporter() {}

//   bool LoadScene(const std::string& file, unsigned int flags) {
//     if (useable) {
//       return false;
//     }

//     loadScene = importer.ReadFile(file, flags);

//     return true;
//   }

//  private:
//   bool useable = false;
//   std::future<const aiScene*> loadScene;
//   Assimp::Importer importer;
// };

// struct ResourceManager::Impl {
//   std::vector<ModelImporter> modelImporters;
//   std::mutex mutex;
// };

// ResourceManager::ResourceManager() {
//   m_impl = std::make_shared<Impl>();
// }

// ResourceManager& ResourceManager::Instance() {
//   static ResourceManager instance;
//   return instance;
// }

// void LoadModelImpl() {}

// ResourceID ResourceManager::LoadModel(const std::string& file) {
//   std::lock_guard<std::mutex> lock(m_impl->mutex);

//   return ResourceID();
// }

// Model& ResourceManager::GetModel(ResourceID id) {
//   return Model();
// }
// void ResourceManager::FreeModel(ResourceID id) {}

// }  // namespace RE::Core