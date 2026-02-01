#pragma once

#include "Resource.h"
#include "IdFactory.h"

namespace re::engine::ecs {
class RE_API ResourceManager {
 public:
  ResourceManager() = default;
  ~ResourceManager() = default;

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

  template <typename T>
  bool HasResource() {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    RE_ASSERT(id < m_Resources.size());
    return m_Resources[id].Get<T>() != nullptr;
  }

  template <typename T>
  RE_FINLINE T* GetResource() {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    RE_ASSERT(id < m_Resources.size());
    return m_Resources[id].Get<T>();
  }

  template <typename T, typename... Args>
  T* CreateResource(Args&&... args) {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    RE_ASSERT(id < m_Resources.size());
    RE_ASSERT(m_Resources[id].Get<void>() == nullptr);

    T* ptr = GAlloc::create<T>(std::forward<Args>(args)...);
    m_Resources[id].Create<T>(ptr);

    return ptr;
  }

  template <typename T>
  void RemoveResource() {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    RE_ASSERT(id < m_Resources.size());
    m_Resources[id].Destroy();
  }

  void Clear() {
    for (auto& res : m_Resources) {
      res.Destroy();
    }
  }

 private:
  array<Resource, ResIdFactory::Max> m_Resources;
};
}  // namespace re::engine::ecs