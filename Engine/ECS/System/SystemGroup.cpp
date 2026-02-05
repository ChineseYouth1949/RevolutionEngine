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

  System::OnAttach(world);
}
void SystemGroup::OnDetach() {
  if (!IsAttach()) {
    return;
  }

  for (auto sys : m_ChildSystem) {
    sys->OnDetach();
  }

  System::OnDetach(world);
}

void SystemGroup::OnEnable() {
  for (auto sys : m_ChildSystem) {
    sys->OnEnable();
  }
  System::OnEnable(world);
}
void SystemGroup::OnDisable() {
  for (auto sys : m_ChildSystem) {
    sys->OnDisable();
  }
  System::OnDisable(world);
}

void SystemGroup::OnPreUpdate() {
  for (auto sys : m_ChildSystem) {
    sys->OnPreUpdate();
  }
}

void SystemGroup::OnPostUpdate() {
  for (auto sys : m_ChildSystem) {
    sys->OnPostUpdate();
  }
}

vector<shared_ptr<SystemPass>> SystemGroup::GetAllPass() {
  vector<shared_ptr<SystemPass>> allPass;
  for (auto sys : m_ChildSystem) {
    auto sysPasses = sys->GetAllPass();
    allPass.insert(allPass.end(), sysPasses.begin(), sysPasses.end());
  }
}

vector<shared_ptr<System>>& SystemGroup::GetChilds() {
  return m_ChildSystem;
}

}  // namespace re::engine::ecs