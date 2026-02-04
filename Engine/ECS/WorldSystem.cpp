#include "WorldSystem.h"

namespace re::engine::ecs {
WorldSystem::WorldSystem() {
  m_World = GAlloc::make_unique<World>();
}
WorldSystem::~WorldSystem() {}

void WorldSystem::Run() {
  if (m_FirstRun) {
    m_TimerDt.Update();
    m_TimerTotal.Update();
    m_FirstRun = false;
  }

  UpdateInfo info;
  info.dt = m_TimerDt.GetTimeMs();
  info.total = m_TimerTotal.GetTimeMs();

  m_TimerDt.Update();

  for (auto& [key, sys] : m_Systems) {
    sys->OnPreUpdate(info);
  }
  for (auto& [key, sys] : m_Systems) {
    sys->OnUpdate(info);
  }
  for (auto& [key, sys] : m_Systems) {
    sys->OnPostUpdate(info);
  }
}

}  // namespace re::engine::ecs