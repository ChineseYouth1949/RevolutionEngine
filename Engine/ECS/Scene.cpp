#include "Scene.h"

namespace re::engine::ecs {
Scene::Scene() {
  m_World = GAlloc::make_unique<World>();
  m_Scheduler = GAlloc::make_unique<PassScheduler>();
}
Scene::~Scene() {}

void Scene::Run() {
  // Recompile if systems have been added/removed
  if (m_SystemsDirty) {
    vector<shared_ptr<Pass>> allPasses;
    for (auto& [key, sys] : m_Systems) {
      auto& passes = sys->GetAllPass();
      allPasses.insert(allPasses.end(), passes.begin(), passes.end());
    }
    m_Scheduler->Compile(allPasses);
    m_SystemsDirty = false;
  }

  // Initialize timers on first run
  if (m_FirstRun) {
    m_TimerDt.Update();
    m_TimerTotal.Update();
    m_FirstRun = false;
  }

  m_TimerDt.Update();

  for (auto& [key, sys] : m_Systems) {
    sys->OnPreUpdate();
  }

  m_Scheduler->Execute();
  m_World->Flush();

  for (auto& [key, sys] : m_Systems) {
    sys->OnPostUpdate();
  }
}

}  // namespace re::engine::ecs