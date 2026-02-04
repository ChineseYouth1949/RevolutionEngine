#pragma once

#include "System.h"

namespace re::engine::ecs {
class SystemGroup : public System {
 public:
  SystemGroup() = default;
  ~SystemGroup() {}

  void AddSystem(shared_ptr<System> sys);

  void OnAttach(World* world) override;
  void OnDetach() override;

  void OnEnable() override;
  void OnDisable() override;

  void OnUpdate() override;

  vector<shared_ptr<System>>& GetChilds() { return m_ChildSystem; }

 protected:
  vector<shared_ptr<System>> m_ChildSystem;
};
}  // namespace re::engine::ecs