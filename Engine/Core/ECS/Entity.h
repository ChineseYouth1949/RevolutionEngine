#pragma once

#include "Engine/Core/Base.h"

#include "WorldBase.h"
#include "State.h"

namespace re::engine::ecs {
class Entity {
 public:
  Entity() {}
  Entity(EntityId id, WorldBase* world) : m_ID(id), m_World(world) {}

  void SetName(const stl::string& str) { m_Name = str; }
  const stl::string& GetName() const { return m_Name; }

  bool Valid() const { return m_World && m_World->GetReg().valid(m_ID); }
  void Destry() { m_World->DestryEntity(m_ID); }

  template <typename... T>
  bool HasComponents() const {
    auto& reg = m_World->GetReg();
    return reg.all_of<T>(m_ID) || reg.all_of<AddComponent<T>>(m_ID);
  }

  template <typename T, typename... Args>
  T& AddComponent(Args&&... args) {
    auto& reg = m_World->GetReg();
    auto& wrapper = reg.emplace_or_replace<AddComponent<T>>(m_ID, std::forward<Args>(args)...);
    return wrapper.data;
  }

  template <typename T>
  const T& GetComponent() const {
    auto& reg = m_World->GetReg();
    if (auto* t = reg.try_get<T>(m_ID)) {
      return *t;
    }
    return reg.get<AddComponent<T>>(m_ID).data;
  }

  template <typename T>
  T& ChangeComponent() {
    auto& reg = m_World->GetReg();
    if (auto* t = reg.try_get<T>(m_ID)) {
      reg.emplace_or_replace<ChangeComponent<T>>(m_ID);
      return *t;
    }
    return reg.get<AddComponent<T>>(m_ID).data;
  }

  template <typename T>
  bool RemoveComponent() {
    auto& reg = m_World->GetReg();
    if (HasComponents<T>()) {
      reg.emplace_or_replace<DelComponent<T>>(m_ID);
      reg.remove<AddComponent<T>>(m_ID);
      return true;
    }
    return false;
  }

 private:
  EntityId m_ID{entt::null};
  WorldBase* m_World{nullptr};
  stl::string m_Name;
};
}  // namespace re::engine::ecs