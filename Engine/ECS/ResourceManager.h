#pragma once

#include "Resource.h"
#include "IdFactory.h"

namespace re::engine::ecs {
class RE_API ResourceManager {
 public:
  ResourceManager() { m_Resource.reserve(1000); }
  ~ResourceManager() = default;

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

  template <typename T>
  bool HasResource() {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    if (id >= m_Resources.size())
      return false;
    return m_Resources[id].Get<T>() != nullptr;
  }

  template <typename T>
  RE_FINLINE T* GetResource() {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    if (id >= m_Resources.size())
      return nullptr;
    return m_Resources[id].Get<T>();
  }

  template <typename T, typename... Args>
  T* CreateResource(Args&&... args) {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    if (id >= m_Resources.size()) {
      m_Resources.resize(id + 1);
    }

    RE_ASSERT(m_Resources[id].Get<void>() == nullptr);

    T* ptr = GAlloc::create<T>(std::forward<Args>(args)...);
    m_Resources[id].Create<T>(ptr);

    return ptr;
  }

  template <typename T>
  void RemoveResource() {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    if (id < m_Resources.size()) {
      m_Resources[id].Destroy();
    }
  }

  void Clear() { m_Resources.clear(); }

 private:
  vector<Resource> m_Resources;
};
}  // namespace re::engine::ecs