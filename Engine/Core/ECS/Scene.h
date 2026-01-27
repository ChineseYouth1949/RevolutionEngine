#pragma once

#include "World.h"
#include "System.h"

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

    if (sys) {
      sys->SetWorld(m_World.get());
      if (sys->OnAttach()) {
        m_Systems[std::type_index(typeid(T))] = sys;
        return true;
      }
    }
    return false;
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
  map<std::type_index, shared_ptr<System>> m_Systems;
  unordered_map<Entity, EntityInfo> m_EntityInfo;
  utility::Timer m_TimerDt;
  utility::Timer m_TimerTotal;
  bool m_FirstRun = true;
};
}  // namespace re::engine::ecs