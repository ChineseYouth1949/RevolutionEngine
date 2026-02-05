#pragma once

#include "Resource/ResourceManager.h"
#include "Resource/ResourceVisitor.h"

#include "World/World.h"
#include "World/RegistryVisitor.h"

#include "System/Pass.h"
#include "System/PassScheduler.h"

#include "System/System.h"
#include "System/SystemGroup.h"

namespace re::engine::ecs {
struct EntityInfo {
  string name;
};

class RE_API Scene {
 public:
  Scene();
  ~Scene();

  World* GetWorld() { return m_World.get(); }

  template <typename T>
  bool HasSystem() {
    return m_Systems.find(std::type_index(typeid(T))) != m_Systems.end();
  }

  template <typename T>
  bool AddSystem(shared_ptr<T> sys) {
    if (HasSystem<T>()) {
      return false;
    }

    sys->OnAttach(m_World);
    m_Systems[std::type_index(typeid(T))] = sys;
    return true;
  }

  template <typename T>
  shared_ptr<T> GetSystem() {
    auto it = m_Systems.find(std::type_index(typeid(T)));
    RE_ASSERT(it != m_Systems.end());
    return std::static_pointer_cast<T>(it->second);
  }

  void Run();

 private:
  unique_ptr<World> m_World;
  unordered_map<Entity, EntityInfo> m_EntityInfo;

  unordered_map<std::type_index, shared_ptr<System>> m_Systems;
  unique_ptr<PassScheduler> m_Scheduler;

  utility::Timer m_TimerDt;
  utility::Timer m_TimerTotal;
  bool m_FirstRun = true;
};
}  // namespace re::engine::ecs