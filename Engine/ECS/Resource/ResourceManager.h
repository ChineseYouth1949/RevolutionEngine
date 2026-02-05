#pragma once

#include "Resource.h"
#include "ResourceCommandBuffer.h"

namespace re::engine::ecs {
class ResourceVisitor;

class RE_API ResourceManager {
 public:
  ResourceManager() = default;
  ~ResourceManager() = default;

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

  RE_FINLINE bool HasResource(ResId id) const { return (id < m_Resources.size() && id.version == m_ResIdVersions[id] && m_Resources[id].Valid()); }
  RE_FINLINE const Resource* GetResource(ResId id) const {
    RE_ASSERT(HasResource(id));
    return &(m_Resources[id]);
  }

  ResId AddResource(Resource& res);
  RE_FINLINE Resource* GetResource(ResId id) { return &(m_Resources[id]); }
  void RemoveResource(ResId id);

  template <typename T>
  void SetTypeResId(ResId id) {
    static const auto type_index = std::type_index(typeid(T));
    m_TypeResIdMap[type_index] = id;
  }

  template <typename T>
  ResId GetTypeResId() const {
    static const auto type_index = std::type_index(typeid(T));
    auto it = m_TypeResIdMap.find(type_index);
    if (it != m_TypeResIdMap.end()) {
      return it->second;
    }
    return ResId::Null;
  }

  RE_FINLINE void SetStringResId(const string& str, ResId id) { m_StringResIdMap[str] = id; }
  RE_FINLINE ResId GetStringResId(const string& str) const {
    auto it = m_StringResIdMap.find(str);
    if (it != m_StringResIdMap.end()) {
      return it->second;
    }
    return ResId::Null;
  }

  ResourceVisitor RequestResourceVisitor();

  void Submit(ResCommandBuffer&& buffer);
  void Flush();

 private:
  void ChangeResourceImpl(stl::pair<ResId, ResOperate>& changeRes);

  vector<Resource> m_Resources;
  vector<uint32_t> m_ResIdVersions;
  vector<ResId> m_FreeResIds;
  vector<ResCommandBuffer> m_CommandBuffers;
  std::mutex m_Mutex;

  unordered_map<std::type_index, ResId> m_TypeResIdMap;
  unordered_map<string, ResId> m_StringResIdMap;
};
}  // namespace re::engine::ecs