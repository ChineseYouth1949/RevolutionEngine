#include "CoreSystem.h"

#include "ColorVertex.h"
#include "RenderAxis.h"

namespace re::engine::render {
using namespace re::engine::ecs;

CoreSystem::CoreSystem() {}
CoreSystem::~CoreSystem() {}

void CoreSystem::Init(engine::shared_ptr<GraphicsCore> gc) {
  m_GC = gc;
  m_SharedInfo = GAlloc::make_shared<SharedInfo>();

  m_SharedInfo->width = 1280;
  m_SharedInfo->height = 720;

  m_SharedInfo->camera.LookAt({0, 2, -5}, {0, 0, 0}, {0, 1, 0});
  float aspectHeightOverWidth = static_cast<float>(1280) / static_cast<float>(720);

  auto renAxis = GAlloc::make_shared<RenderAxis>();
  renAxis->Init(m_GC, m_SharedInfo);
  AddChild(renAxis);

  auto renColorVertex = GAlloc::make_shared<RenderColorVertex>();
  renColorVertex->Init(m_GC, m_SharedInfo);
  AddChild(renColorVertex);
}

void CoreSystem::OnAttach(ecs::World* world) {
  SystemGroup::OnAttach(world);
}
void CoreSystem::OnDetach() {
  // Ensure all GPU work is complete before detaching render system
  auto* ctx = m_GC->GetGraphicsContext();
  if (ctx != nullptr) {
    ctx->Finish(true);  // Wait for GPU to complete all pending work
  }
  SystemGroup::OnDetach();
}

void CoreSystem::OnEnable() {
  SystemGroup::OnEnable();
}
void CoreSystem::OnDisable() {
  SystemGroup::OnDisable();
}

void CoreSystem::OnPreUpdate() {
  m_GC->Begin();
  SystemGroup::OnPreUpdate();
}
void CoreSystem::OnPostUpdate() {
  SystemGroup::OnPostUpdate();
  m_GC->End();
}

}  // namespace re::engine::render