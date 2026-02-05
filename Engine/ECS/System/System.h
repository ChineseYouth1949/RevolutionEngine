#pragma once

#include "SystemPass.h"

namespace re::engine::ecs {
class World;

class System {
 public:
  System() = default;
  virtual ~System() = default;

  System(const System&) = delete;
  System& operator=(const System&) = delete;

  RE_FINLINE bool IsAttach() const { return m_Attach; }
  RE_FINLINE bool IsEnabled() const { return m_Enabled; }

  virtual void OnAttach(World* world) {
    m_World = world;
    m_Attach = true;
    for (auto& pass : m_Passes) {
      pass->Init(world);
    }
  }
  virtual void OnDetach() {
    m_World = nullptr;
    m_Attach = false;
  }

  virtual void OnEnable() { m_Enabled = true; }
  virtual void OnDisable() { m_Enabled = false; }

  virtual void OnPreUpdate() {}
  virtual void OnPostUpdate() {}

  RE_FINLINE vector<shared_ptr<SystemPass>> GetAllPass() { return m_Passes; }

 protected:
  template <typename ComponentType>
  bool PollAddComponent();

  template <typename ComponentType>
  bool PollDelComponent();

  template <typename ComponentType>
  bool PollChangeComponent();

  World* m_World = nullptr;
  bool m_Enabled = false;
  bool m_Attach = false;

  vector<shared_ptr<SystemPass>> m_Passes;
};

}  // namespace re::engine::ecs

namespace re::engine::ecs {}

#include "System.inl"