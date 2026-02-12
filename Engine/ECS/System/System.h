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

  virtual bool OnAttach(World* world) {
    if (world && m_Attach == false) {
      m_World = world;
      m_Attach = true;
      for (auto& pass : m_Passes) {
        pass->Init(world);
      }
    }
    return m_Attach;
  }
  virtual bool OnDetach() {
    if (m_Attach) {
      m_World = nullptr;
      m_Attach = false;
    }
    return !m_Attach;
  }

  virtual bool OnEnable() {
    if (!m_Enabled) {
      m_Enabled = true;
    }
    return m_Enabled;
  }
  virtual bool OnDisable() {
    if (m_Enabled) {
      m_Enabled = true;
    }
    return !m_Enabled;
  }

  virtual void OnPreUpdate() {}
  virtual void OnPostUpdate() {}

  RE_FINLINE vector<shared_ptr<SystemPass>> GetAllPass() { return m_Passes; }

 protected:
  template <typename ComponentType>
  bool PollAddComponentTemp();

  template <typename ComponentType>
  bool PollDelComponentTemp();

  template <typename ComponentType>
  bool PollChangeComponentTemp();

  World* m_World = nullptr;
  bool m_Enabled = false;
  bool m_Attach = false;

  vector<shared_ptr<SystemPass>> m_Passes;
};

}  // namespace re::engine::ecs

namespace re::engine::ecs {}

#include "System.inl"