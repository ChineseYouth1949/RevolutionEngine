#pragma once

namespace re::engine::ecs {
class World;

class System {
 public:
  System() = default;
  virtual ~System() = default;

  System(const System&) = delete;
  System& operator=(const System&) = delete;

  void SetWorld(World* world) { m_World = world; }
  World* GetWorld() const { return m_World; }

  bool IsAttach() const { return m_Attach; }
  bool IsEnabled() const { return m_Enabled; }

  virtual bool OnAttach() = 0;
  virtual bool OnDetach() = 0;

  virtual void OnEnable() = 0;
  virtual void OnDisable() = 0;

  virtual void OnPreUpdate() {}
  virtual void OnUpdate() = 0;
  virtual void OnPostUpdate() {}

 protected:
  template <typename ComponentType>
  bool PollAddComponent();

  template <typename ComponentType>
  bool PollDelComponent();

  template <typename ComponentType>
  bool PollChangeComponent();

  bool m_Enabled = false;
  bool m_Attach = false;
  World* m_World = nullptr;
};

}  // namespace re::engine::ecs

namespace re::engine::ecs {}

#include "System.inl"