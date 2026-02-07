#pragma once

#include "Engine/ECS/Resource/ResourceManager.h"

#include "ComponentTag.h"
#include "Transform.h"
#include "RegistryCommandBuffer.h"

namespace re::engine::ecs {
class RegistryVisitor;

class RE_API World {
 public:
  World();
  virtual ~World();

  RE_FINLINE bool HasEntity(Entity e) const { return m_Reg.valid(e); }
  RE_FINLINE Entity CreateEntity() {
    Entity e = m_Reg.create();
    m_Reg.emplace_or_replace<Transform>(e);
    return e;
  }

  RE_FINLINE void DestroyEntity(Entity e, bool delay = false) {
    if (delay) {
      m_DestryEntitys.push_back(e);
    } else {
      m_Reg.destroy(e);
    }
  }

  template <typename T>
  bool HasComponents(Entity e) const {
    return m_Reg.all_of<T>(e);
  }

  template <typename T>
  T& GetComponent(Entity e) {
    return m_Reg.get<T>(e);
  }

  template <typename T>
  const T& GetComponent(Entity e) const {
    return m_Reg.get<T>(e);
  }

  template <typename T, typename... Args>
  T& AddComponent(Entity e, Args&&... args) {
    return m_Reg.emplace_or_replace<T>(e, std::forward<Args>(args)...);
  }

  template <typename T, typename... Args>
  T& AddComponentDelay(Entity e, Args&&... args) {
    auto& wrapper = m_Reg.emplace_or_replace<AddComponentTag<T>>(e, std::forward<Args>(args)...);
    return wrapper.data;
  }

  template <typename T, typename... Args>
  void ChangeComponent(Entity e, Args&&... args) {
    m_Reg.emplace_or_replace<T>(e, std::forward<Args>(args)...);
  }

  template <typename T, typename... Args>
  void ChangeComponentDelay(Entity e, Args&&... args) {
    m_Reg.emplace_or_replace<ChangeComponentTag<T>>(e, std::forward<Args>(args)...);
  }

  template <typename T>
  bool RemoveComponent(Entity e) {
    return m_Reg.remove<T>(e) > 0;
  }

  template <typename T>
  bool RemoveComponentDelay(Entity e) {
    if (HasComponents<T>(e)) {
      m_Reg.emplace_or_replace<DelComponentTag<T>>(e);
      m_Reg.remove<AddComponentTag<T>>(e);
      return true;
    }
    return false;
  }

  RE_FINLINE Registry* GetRegistry() { return &m_Reg; }
  RE_FINLINE ResourceManager* GetResourceManager() { return m_ResourceManager.get(); }

  RegistryVisitor RequestRegistryVisitor();

  void Submit(RegistryCommandBuffer&& buffer);
  void Flush();

 protected:
  Registry m_Reg;
  vector<Entity> m_DestryEntitys;
  unique_ptr<ResourceManager> m_ResourceManager;

  std::mutex m_SumbitMutex;
  vector<RegistryCommandBuffer> m_CommandBuffers;
};
}  // namespace re::engine::ecs