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

  bool HasResource(ResourceId id) const { return (id < m_Resources.size() && m_Resources[id].Valid()); }
  const Resource& GetResource(ResourceId id) const { return m_Resources[id]; }

  void Submit(ResCommandBuffer& buffer);
  void Flush();
  void Reset();

 private:
  void AddResource(stl::pair<shared_ptr<ResourceId>, Resource>& addRes);
  void ChangeResource(stl::pair<ResourceId, ResOperate>& changeRes);
  void RemoveResource(ResourceId id);

  vector<Resource> m_Resources;
  vector<ResourceId> m_FreeResIds;
  vector<ResCommandBuffer> m_CommandBuffers;
  std::mutex m_Mutex;
};
}  // namespace re::engine::ecs