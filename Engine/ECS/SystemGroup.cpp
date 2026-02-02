#include "SystemGroup.h"

namespace re::engine::ecs {
void SystemGroup::AddSystem(shared_ptr<System> sys) {
  m_ChildSystem.push_back(sys);
}

bool SystemGroup::OnAttach() {
  if (IsAttach()) {
    return false;
  }

  bool success = true;
  for (auto sys : m_ChildSystem) {
    if (!sys->OnAttach()) {
      success = false;
      break;
    }
  }

  if (!success) {
    for (auto sys : m_ChildSystem) {
      if (sys->IsAttach()) {
        RE_ASSERT(sys->OnDetach());
      }
    }
  }

  m_Attach = success;
  return success;
}
bool SystemGroup::OnDetach() {
  if (!IsAttach()) {
    return false;
  }

  bool success = true;
  for (auto sys : m_ChildSystem) {
    if (sys->OnDetach()) {
      success = false;
      break;
    }
  }

  if (!success) {
    for (auto sys : m_ChildSystem) {
      if (!sys->IsAttach()) {
        RE_ASSERT(sys->OnAttach());
      }
    }
  }

  m_Attach = success;
  return success;
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

void SystemGroup::OnPreUpdate() {
  for (auto sys : m_ChildSystem) {
    sys->OnPreUpdate();
  }
}
void SystemGroup::OnUpdate() {
  for (auto sys : m_ChildSystem) {
    sys->OnUpdate();
  }
}
void SystemGroup::OnPostUpdate() {
  for (auto sys : m_ChildSystem) {
    sys->OnPostUpdate();
  }
}

}  // namespace re::engine::ecs