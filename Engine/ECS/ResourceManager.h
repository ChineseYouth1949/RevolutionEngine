#pragma once

#include "Resource.h"
#include "ResourceCommandBuffer.h"

namespace re::engine::ecs {
class RE_API ResourceManager {
 public:
  ResourceManager() = default;
  ~ResourceManager() = default;

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

  RE_FINLINE bool HasResource(ResourceId id) const { return (id < m_Resources.size() && m_Resources[id].Valid()); }
  RE_FINLINE const Resource& GetResource(ResourceId id) const { return m_Resources[id]; }

  ResourceId AddResource(Resource&);
  RE_FINLINE Resource& GetResource(ResourceId id) { return m_Resources[id]; }
  void RemoveResource(ResourceId);

  template <typename T>
  void SetTypeResource(ResourceId id) {
    static const auto type_index = std::type_index(typeid(T));
    m_TypeResIdMap[type_index] = id;
  }

  template <typename T>
  const T* GetTypeResource() {
    static const auto type_index = std::type_index(typeid(T));
    const auto id = m_TypeResIdMap[type_index];
    return m_Resources[id].Get<T>();
  }

  void Submit(ResCommandBuffer& buffer);
  void Flush();
  void Reset();

 private:
  void ChangeResource(stl::pair<ResourceId, ResOperate>& changeRes);

  vector<Resource> m_Resources;
  vector<ResourceId> m_FreeResIds;
  vector<ResCommandBuffer> m_CommandBuffers;
  std::mutex m_Mutex;

  unordered_map<std::type_index, ResourceId> m_TypeResIdMap;
};
}  // namespace re::engine::ecs