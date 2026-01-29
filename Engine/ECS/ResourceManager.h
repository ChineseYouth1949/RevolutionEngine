#pragma once

#include "Engine/Base/All.h"

#include "Resource.h"
#include "Engine/Base/Utility/TypeId.h"

namespace re::engine::ecs {
class RE_API ResourceManager {
 public:
  ResourceManager() = default;
  ~ResourceManager() { Clear(); }

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

  template <typename T, typename... Args>
  T* CreateResource(Args&&... args) {
    const size_t id = TypeId<T>();

    if (id >= m_Resources.size()) {
      m_Resources.resize(id + 1);
    }

    T* ptr = GAlloc::create<T>(std::forward<Args>(args)...);

    m_Resources[id].Create<T>(ptr);
    return ptr;
  }

  template <typename T>
  RE_FINLINE T* GetResource() {
    const size_t id = TypeId<T>();
    if (id < m_Resources.size()) {
      return m_Resources[id].Get<T>();
    }
    return nullptr;
  }

  template <typename T>
  void RemoveResource() {
    const size_t id = TypeId<T>();
    if (id < m_Resources.size()) {
      m_Resources[id].Destroy();
    }
  }

  template <typename T>
  bool HasResource() {
    const size_t id = TypeId<T>();
    return (id < m_Resources.size() && m_Resources[id].m_Ptr != nullptr);
  }

  void Clear() { m_Resources.clear(); }

 private:
  vector<Resource> m_Resources;
};
}  // namespace re::engine::ecs