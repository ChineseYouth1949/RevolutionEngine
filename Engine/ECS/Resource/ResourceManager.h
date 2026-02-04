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

  RE_FINLINE bool HasResource(ResId id) const { return (id < m_Resources.size() && m_Resources[id].Valid()); }
  RE_FINLINE const Resource* GetResource(ResId id) const { return &(m_Resources[id]); }

  void Submit(ResCommandBuffer& buffer);

  // Do not use in multi-threaded environments
  ResId AddResource(Resource& res);
  RE_FINLINE Resource* GetResource(ResId id) { return &(m_Resources[id]); }
  void RemoveResource(ResId id);

  template <typename T>
  void SetTypeRes(ResId id) {
    static const auto type_index = std::type_index(typeid(T));
    m_TypeResIdMap[type_index] = id;
  }

  template <typename T>
  Resource* GetTypeRes() {
    static const auto type_index = std::type_index(typeid(T));
    auto it = m_TypeResIdMap.find(type_index);
    if (it != m_TypeResIdMap.end()) {
      return &(m_Resources[it->second]);
    }
    return nullptr;
  }

  void Flush();

 private:
  void ChangeResourceImpl(stl::pair<ResId, ResOperate>& changeRes);

  vector<Resource> m_Resources;
  vector<ResId> m_FreeResIds;
  vector<ResCommandBuffer> m_CommandBuffers;
  std::mutex m_Mutex;

  unordered_map<std::type_index, ResId> m_TypeResIdMap;
};
}  // namespace re::engine::ecs