#include "RenderSystem.h"

#include "ColorVertex.h"
#include "RenderAxis.h"

namespace re::engine::render {
using namespace re::engine::ecs;

RenderSystem::RenderSystem() {}
RenderSystem::~RenderSystem() {}

void RenderSystem::Init(engine::shared_ptr<GraphicsCore> gc) {
  m_GC = gc;
  m_SharedInfo = re::engine::shared_ptr<SharedInfo>(new SharedInfo());

  m_SharedInfo->width = 1280;
  m_SharedInfo->height = 720;

  m_SharedInfo->camera.LookAt({0, 2, -5}, {0, 0, 0}, {0, 1, 0});
  float aspectHeightOverWidth = static_cast<float>(1280) / static_cast<float>(720);

  re::engine::shared_ptr<RenderColorVertex> renColorVertex(new RenderColorVertex());
  renColorVertex->Init(m_GC, m_SharedInfo);
  AddSystem(renColorVertex);

  re::engine::shared_ptr<RenderAxis> renAxis(new RenderAxis());
  renAxis->Init(m_GC, m_SharedInfo);
  AddSystem(renAxis);
}

void RenderSystem::OnAttach(ecs::World* world) {
  SystemGroup::OnAttach(world);
}
void RenderSystem::OnDetach() {
  // Ensure all GPU work is complete before detaching render system
  auto* ctx = m_GC->GetGraphicsContext();
  if (ctx != nullptr) {
    ctx->Finish(true);  // Wait for GPU to complete all pending work
  }
  SystemGroup::OnDetach();
}

void RenderSystem::OnEnable() {
  SystemGroup::OnEnable();
}
void RenderSystem::OnDisable() {
  SystemGroup::OnDisable();
}

void RenderSystem::OnPreUpdate() {
  SystemGroup::OnPreUpdate();
}
void RenderSystem::OnPostUpdate() {
  SystemGroup::OnPostUpdate();
}

}  // namespace re::engine::render