#pragma once

#include "WorldBase.h"

namespace re::engine::ecs {

struct UpdateInfo {
  float dt;
};

class System {
 public:
  System() = default;
  virtual ~System() = default;

  System(const System&) = delete;
  System& operator=(const System&) = delete;

  void SetWorld(WorldBase* world) { m_World = world; }
  WorldBase* GetWorld() const { return m_World; }

  virtual bool Initialize() = 0;
  virtual bool Release() = 0;

  virtual void OnEnable() {}
  virtual void OnDisable() {}

  virtual void PreUpdate(const UpdateInfo& info) = 0;
  virtual void Update(const UpdateInfo& info) = 0;
  virtual void PostUpdate(const UpdateInfo& info) = 0;

 protected:
  WorldBase* m_World = nullptr;
};

}  // namespace re::engine::ecs
