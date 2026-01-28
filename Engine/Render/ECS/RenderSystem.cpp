#include "RenderSystem.h"

#include "ColorVertex.h"
#include "RenderAxis.h"

namespace re::engine::render {
using namespace re::engine::ecs;

RenderSystem::RenderSystem() {}
RenderSystem::~RenderSystem() {}

void RenderSystem::Init(shared_ptr<GraphicsCore> gc) {
  m_GC = gc;
  m_SharedInfo = GAlloc::make_shared<SharedInfo>();

  m_SharedInfo->width = 1280;
  m_SharedInfo->height = 720;

  m_SharedInfo->camera.LookAt({0, 2, -5}, {0, 0, 0}, {0, 1, 0});
  float aspectHeightOverWidth = static_cast<float>(1280) / static_cast<float>(720);

  auto renColorVertex = GAlloc::make_shared<RenderColorVertex>();
  renColorVertex->Init(m_GC, m_SharedInfo);
  AddSystem(renColorVertex);

  auto renAxis = GAlloc::make_shared<RenderAxis>();
  renAxis->Init(m_GC, m_SharedInfo);
  AddSystem(renAxis);
}

bool RenderSystem::OnAttach() {
  return SystemGroup::OnAttach();
}
bool RenderSystem::OnDetach() {
  return SystemGroup::OnDetach();
}

void RenderSystem::OnEnable() {
  SystemGroup::OnEnable();
}
void RenderSystem::OnDisable() {
  SystemGroup::OnDisable();
}

void RenderSystem::OnPreUpdate(const UpdateInfo& info) {
  SystemGroup::OnPreUpdate(info);
}
void RenderSystem::OnUpdate(const UpdateInfo& info) {
  m_GC->Begin();
  SystemGroup::OnUpdate(info);
  m_GC->End();
}
void RenderSystem::OnPostUpdate(const UpdateInfo& info) {
  SystemGroup::OnPostUpdate(info);
}

}  // namespace re::engine::render