#pragma once

#include "ComponentTag.h"
#include "ResourceManager.h"

namespace re::engine::ecs {
using Entity = entt::entity;
using Registry = entt::basic_registry<Entity, GAllocSTL<Entity>>;

class RE_API World {
 public:
  World();
  virtual ~World();

  RE_FINLINE Registry* GetRegistry() { return &m_Reg; }
  RE_FINLINE ResourceManager* GetResourceManager() { return m_ResourceManager.get(); }

  RE_FINLINE bool HasEntity(Entity e) const { return m_Reg.valid(e); }
  RE_FINLINE shared_ptr<Entity> CreateEntity() const {
    auto ePtr = GAlloc::make_shared<Entity>();
    m_AddEntitys.push_back(ePtr);
    return ePtr;
  }
  RE_FINLINE void DestryEntity(Entity e) {
    std::lock_guard<std::mutex> lock(m_DestroyMutex);
    m_DestryEntitys.push_back(e);
  }

  template <typename... T>
  RE_FINLINE bool HasComponents(Entity e) const {
    return m_Reg.all_of<T>(e);
  }

  template <typename T, typename... Args>
  T& AddComponent(Entity e, Args&&... args) {
    auto& wrapper = m_Reg.emplace_or_replace<AddComponentTag<T>>(e, std::forward<Args>(args)...);
    return wrapper.data;
  }

  template <typename T>
  RE_FINLINE T& GetComponent(Entity e) {
    return m_Reg.get<T>(e);
  }

  template <typename T>
  RE_FINLINE const T& GetComponent(Entity e) const {
    return m_Reg.get<T>(e);
  }

  template <typename T>
  T& ChangeComponent(Entity e) {
    m_Reg.emplace_or_replace<ChangeComponentTag<T>>(e);
    return m_Reg.get<T>(e);
  }

  template <typename T>
  bool RemoveComponent(Entity e) {
    if (HasComponents<T>(e)) {
      m_Reg.emplace_or_replace<DelComponentTag<T>>(e);
      m_Reg.remove<AddComponentTag<T>>(e);
      return true;
    }
    return false;
  }

 protected:
  Registry m_Reg;

  vector<shared_ptr<Entity>> m_AddEntitys;
  vector<Entity> m_DestryEntitys;

  unique_ptr<ResourceManager> m_ResourceManager;
};
}  // namespace re::engine::ecs