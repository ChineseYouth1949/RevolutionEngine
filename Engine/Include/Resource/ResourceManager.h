#pragma once

#include <memory>

#include "ResourceInfo.h"

namespace RE::Core {
class Model;

class ResourceManager {
 public:
  static ResourceManager& Instance();

  ResourceID LoadModel();
  Model& GetModel(ResourceID id);
  void FreeModel(ResourceID id);

 private:
  ResourceManager();

  struct Impl;
  std::shared_ptr<Impl> m_impl;
};

}  // namespace RE::Core
