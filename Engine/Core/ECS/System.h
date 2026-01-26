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

class SystemGroup : public System {
 public:
  SystemGroup() = default;
  ~SystemGroup() { Release(); }

  bool Initialize() override {
    for (auto sys : m_ChildSystem) {
      if (!sys->Initialize()) {
        return false;
      }
    }
    return true;
  }
  bool Release() override {
    for (auto sys : m_ChildSystem) {
      if (sys->Release()) {
        return false;
      }
    }
    m_ChildSystem.clear();
    return true;
  }

  void OnEnable() {
    for (auto sys : m_ChildSystem) {
      sys->OnEnable();
    }
  }
  void OnDisable() {
    for (auto sys : m_ChildSystem) {
      sys->OnDisable();
    }
  }

  void PreUpdate(const UpdateInfo& info) {
    for (auto sys : m_ChildSystem) {
      sys->PreUpdate(info);
    }
  }
  void Update(const UpdateInfo& info) {
    for (auto sys : m_ChildSystem) {
      sys->Update(info);
    }
  }
  void PostUpdate(const UpdateInfo& info) {
    for (auto sys : m_ChildSystem) {
      sys->PostUpdate(info);
    }
  }

 protected:
  Alloc::vector<System*> m_ChildSystem;
};

}  // namespace re::engine::ecs
