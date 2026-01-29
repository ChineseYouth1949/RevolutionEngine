#pragma once

#include "System.h"

namespace re::engine::ecs {
class SystemGroup : public System {
 public:
  SystemGroup() = default;
  ~SystemGroup() {}

  void AddSystem(shared_ptr<System> sys);

  bool OnAttach() override;
  bool OnDetach() override;

  void OnEnable() override;
  void OnDisable() override;

  void OnPreUpdate(const UpdateInfo& info) override;
  void OnUpdate(const UpdateInfo& info) override;
  void OnPostUpdate(const UpdateInfo& info) override;

  vector<shared_ptr<System>>& GetChilds() { return m_ChildSystem; }

 protected:
  vector<shared_ptr<System>> m_ChildSystem;
};
}  // namespace re::engine::ecs