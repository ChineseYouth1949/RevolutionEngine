#include "Scene.h"

namespace re::engine::ecs {
Scene::Scene() {
  m_World = GAlloc::make_unique<World>();
}
Scene::~Scene() {}

void Scene::Run() {
  if (m_FirstRun) {
    m_TimerDt.Update();
    m_TimerTotal.Update();
    m_FirstRun = false;
  }

  // UpdateInfo info;
  // info.dt = m_TimerDt.GetTimeMs();
  // info.total = m_TimerTotal.GetTimeMs();

  m_TimerDt.Update();

  for (auto& [key, sys] : m_Systems) {
    sys->OnUpdate();
  }

  vector<shared_ptr<Stage>> stages;
  for (auto& [key, sys] : m_Systems) {
    auto& sts = sys->GetAllStage();
    stages.insert(stages.end(), sts.begin(), sts.end());
  }

  m_Scheduler->Compile(stages);
  m_Scheduler->Execute();
}

}  // namespace re::engine::ecs