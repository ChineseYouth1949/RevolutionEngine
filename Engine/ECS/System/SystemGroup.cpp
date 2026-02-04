#include "SystemGroup.h"

namespace re::engine::ecs {
void SystemGroup::AddSystem(shared_ptr<System> sys) {
  m_ChildSystem.push_back(sys);
}

void SystemGroup::OnAttach(World* world) {
  if (IsAttach()) {
    return;
  }

  for (auto sys : m_ChildSystem) {
    sys->OnAttach(world);
  }

  m_Attach = true;
}
void SystemGroup::OnDetach() {
  if (!IsAttach()) {
    return;
  }

  for (auto sys : m_ChildSystem) {
    sys->OnDetach();
  }

  m_Attach = false;
}

void SystemGroup::OnEnable() {
  for (auto sys : m_ChildSystem) {
    sys->OnEnable();
  }
}
void SystemGroup::OnDisable() {
  for (auto sys : m_ChildSystem) {
    sys->OnDisable();
  }
}

void SystemGroup::OnUpdate() {
  for (auto sys : m_ChildSystem) {
    sys->OnUpdate();
  }
}

}  // namespace re::engine::ecs