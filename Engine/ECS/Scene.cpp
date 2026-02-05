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

    vector<shared_ptr<Pass>> allPasses;
    for (auto& [key, sys] : m_Systems) {
      auto& passes = sys->GetAllPass();
      allPasses.insert(allPasses.end(), passes.begin(), passes.end());
    }

    m_Scheduler->Compile(allPasses);

    m_FirstRun = false;
  }

  // UpdateInfo info;
  // info.dt = m_TimerDt.GetTimeMs();
  // info.total = m_TimerTotal.GetTimeMs();

  m_TimerDt.Update();

  for (auto& [key, sys] : m_Systems) {
    sys->OnPreUpdate();
  }

  m_Scheduler->Execute();

  for (auto& [key, sys] : m_Systems) {
    sys->OnPostUpdate();
  }
}

}  // namespace re::engine::ecs