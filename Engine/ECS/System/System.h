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

  virtual void OnAttach(World* world) = 0;
  virtual void OnDetach() = 0;

  virtual void OnEnable() = 0;
  virtual void OnDisable() = 0;

  virtual void OnPreUpdate() = 0;
  virtual void OnPostUpdate() = 0;

  RE_FINLINE vector<shared_ptr<SystemPass>>& GetAllStage() { return m_Passes; }

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