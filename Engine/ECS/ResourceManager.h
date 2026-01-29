#pragma once

#include "Resource.h"

namespace re::engine::ecs {
RE_DEFINE_TYPE_ID_FACTORY(ResTypeIdFactory)

using ResouceId = ResTypeIdFactory::ValueType;

class RE_API ResourceManager {
 public:
  ResourceManager() = default;
  ~ResourceManager() { Clear(); }

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

  template <typename T, typename... Args>
  T* CreateResource(Args&&... args) {
    const ResouceId id = ResTypeIdFactory::Get<T>();

    if (id >= m_Resources.size()) {
      m_Resources.resize(id + 1);
    }

    T* ptr = GAlloc::create<T>(std::forward<Args>(args)...);

    m_Resources[id].Create<T>(ptr);
    return ptr;
  }

  template <typename T>
  RE_FINLINE T* GetResource() {
    const ResouceId id = ResTypeIdFactory::Get<T>();
    if (id < m_Resources.size()) {
      return m_Resources[id].Get<T>();
    }
    return nullptr;
  }

  template <typename T>
  void RemoveResource() {
    const ResouceId id = ResTypeIdFactory::Get<T>();
    if (id < m_Resources.size()) {
      m_Resources[id].Destroy();
    }
  }

  template <typename T>
  bool HasResource() {
    const ResouceId id = ResTypeIdFactory::Get<T>();
    return (id < m_Resources.size() && m_Resources[id].m_Ptr != nullptr);
  }

  void Clear() { m_Resources.clear(); }

 private:
  vector<Resource> m_Resources;
};
}  // namespace re::engine::ecs