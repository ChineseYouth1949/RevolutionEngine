#pragma once

#include "Engine/ECS/Resource/ResourceManager.h"

#include "ComponentTag.h"
#include "RegistryCommandBuffer.h"

namespace re::engine::ecs {
class RegistryVisitor;

class RE_API World {
 public:
  World();
  virtual ~World();

  RE_FINLINE bool HasEntity(Entity e) const { return m_Reg.valid(e); }
  RE_FINLINE Entity CreateEntity() { return m_Reg.create(); }
  RE_FINLINE void DestryEntity(Entity e, bool dely = false) {
    if (dely) {
      m_DestryEntitys.push_back(e);
    } else {
      m_Reg.destroy(e);
    }
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

  template <typename T>
  RE_FINLINE T& GetComponent(Entity e) {
    RE_ASSERT(HasComponents<T>(e), "Component not found on entity. Did you call Flush()?");
    return m_Reg.get<T>(e);
  }

  template <typename T>
  RE_FINLINE const T& GetComponent(Entity e) const {
    RE_ASSERT(HasComponents<T>(e), "Component not found on entity. Did you call Flush()?");
    return m_Reg.get<T>(e);
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