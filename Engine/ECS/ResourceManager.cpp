#include "ResourceManager.h"

namespace re::engine::ecs {
void ResourceManager::Submit(ResCommandBuffer& buffer) {
  std::lock_guard lock(m_Mutex);
  m_CommandBuffers.push_back(std::move(buffer));
}

ResourceId ResourceManager::AddResource(Resource& res) {
  ResourceId id;
  if (m_FreeResIds.empty()) {
    id = m_Resources.size();
    m_Resources.push_back(std::move(res));
  } else {
    id = m_FreeResIds.back();
    m_FreeResIds.pop_back();
    m_Resources[id] = std::move(res);
  }
  return id;
}
void ResourceManager::RemoveResource(ResourceId id) {
  m_FreeResIds.push_back(id);
  m_Resources[id].Destroy();
}

void ResourceManager::Flush() {
  uint32_t addResNum = 0;
  uint32_t removeResNum = 0;

  for (auto& cmb : m_CommandBuffers) {
    addResNum += cmb.GetAddResources().size();
    removeResNum += cmb.GetRemoveResources().size();
  }

  int absAddResNum = addResNum - removeResNum;
  if (absAddResNum > 0) {
    m_Resources.reserve(m_Resources.size() + absAddResNum);
  }

  for (auto& cmb : m_CommandBuffers) {
    auto& addResArray = cmb.GetAddResources();
    auto& changeResArray = cmb.GetChangeResources();
    auto& removeResArray = cmb.GetRemoveResources();

    auto& opOrders = cmb.GetOpOrders();
    for (auto& op : opOrders) {
      if (op.first == ResCommandBuffer::OpType::Add) {
        addResArray[op.second].first->value = AddResource(addResArray[op.second].second);
      } else if (op.first == ResCommandBuffer::OpType::Change) {
        ChangeResource(changeResArray[op.second]);
      } else if (op.first == ResCommandBuffer::OpType::Remove) {
        RemoveResource(removeResArray[op.second]);
      }
    }

    cmb.Reset();
  }

  m_CommandBuffers.clear();
}

void ResourceManager::ChangeResource(stl::pair<ResourceId, ResOperate>& changeRes) {
  changeRes.second(m_Resources[changeRes.first]);
}

void ResourceManager::Reset() {
  m_Resources.clear();
  m_FreeResIds.clear();
  m_CommandBuffers.clear();
}

}  // namespace re::engine::ecs