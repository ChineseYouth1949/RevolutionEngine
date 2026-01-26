#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Memory/Allocator.h"

#include "WorldBase.h"
#include "Entity.h"
#include "System.h"

namespace re::engine::ecs {
class RE_API World : public WorldBase {
 public:
  World();
  ~World();

  RE_FINLE Entity CreateEntity() { return Entity(m_Reg.create(), this); }

  template <typename T>
  bool AddSystem(T* system) {
    static_assert(std::is_base_of<System, T>::value, "T must derive from System");

    auto index = std::type_index(typeid(T));
    if (m_Systems.find(index) != m_Systems.end()) {
      return false;
    }

    system->SetWorld(this);
    m_Systems[index] = system;
    return true;
  }

  bool Initialize() {
    for (auto [tag, sys] : m_Systems) {
      if (sys->Initialize()) {
        return false;
      }
    }
    return true;
  }

 private:
  Alloc::map<uint32_t, System*> m_Systems;
};
}  // namespace re::engine::ecs