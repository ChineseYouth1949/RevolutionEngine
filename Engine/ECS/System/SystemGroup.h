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

  void OnPreUpdate() override;
  void OnPostUpdate() override;

  vector<shared_ptr<SystemPass>> GetAllPass();
  vector<shared_ptr<System>>& GetChilds();

 protected:
  vector<shared_ptr<System>> m_ChildSystem;
};
}  // namespace re::engine::ecs