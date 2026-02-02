#pragma once

#include "Resource.h"
#include "DescriptorFun.h"

namespace re::engine::ecs {
template <typename Type>
RE_FINLINE static const utility::Descriptor& GetResDescriptor() {
  return utility::DescriptorFactory::GetAt<Resource, Type>();
}

class RE_API ResourceManager {
 public:
  ResourceManager() = default;
  ~ResourceManager() = default;

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

  bool HasResource(uint32_t id) const {
    if (id >= m_Resources.size())
      return false;
    return m_Resources[id] != nullptr && m_Resources[id]->GetPtr() != nullptr;
  }

  template <typename T>
  bool HasTypeResource() const {
    static const auto id = GetResDescriptor<T>().Id();
    return HasResource(id);
  }

  RE_FINLINE Resource* GetResource(uint32_t id) {
    if (!HasResource(id)) {
      return nullptr;
    }
    return m_Resources[id].get();
  }

  template <typename T>
  RE_FINLINE Resource* GetTypeResource() {
    static const auto id = GetResDescriptor<T>().Id();
    return GetResource(id);
  }

  uint32_t CreateResource(Resource* resouce) {
    static const auto id = utility::DescriptorFactory::Create().Id();
    if (id >= m_Resources.size()) {
      m_Resources.resize(id + 1);
    }

    RE_ASSERT(m_Resources[id].Get<void>() == nullptr);

    T* ptr = GAlloc::create<T>(std::forward<Args>(args)...);
    m_Resources[id].Create<T>(ptr);

    return ptr;
  }

  template <typename T, typename... Args>
  T* CreateResource(Args&&... args) {
    static const auto id = GetResDescriptor<T>().Id();
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
    static const auto id = GetResDescriptor<T>().Id();
    if (id < m_Resources.size()) {
      m_Resources[id].Destroy();
    }
  }

  void Clear() { m_Resources.clear(); }

 private:
  vector<unique_ptr<Resource>> m_Resources;
};
}  // namespace re::engine::ecs