#pragma once

#include "World.h"

namespace re::engine::ecs {

struct UpdateInfo {
  float dt;
  double total;
};

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

  virtual void OnPreUpdate(const UpdateInfo& info) {}
  virtual void OnUpdate(const UpdateInfo& info) = 0;
  virtual void OnPostUpdate(const UpdateInfo& info) {}

 protected:
  World* m_World = nullptr;
  bool m_Enabled = false;
  bool m_Attach = false;
};

}  // namespace re::engine::ecs
