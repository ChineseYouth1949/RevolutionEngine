#include "Engine/Render/RHI/GraphicsCore.h"
#include "RenderSystem.h"

#include "ColorVertex.h"
#include "ConstBuffer.h"

namespace re::engine::render {
using namespace re::engine::ecs;

RenderSystem::RenderSystem() {}
RenderSystem::~RenderSystem() {}

void RenderSystem::Init(shared_ptr<GraphicsCore> gc) {
  m_GC = gc;

  auto renColorVertex = GAlloc::make_shared<RenderColorVertex>();
  renColorVertex->Init(m_GC);

  AddSystem(renColorVertex);
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
  auto pGfxContext = m_GC->GetGraphicsContext();

  CameraCB cameraCB;
  cameraCB.view = m_Camera.GetViewMatrix();
  cameraCB.proj = m_Camera.GetProjMatrix();
  cameraCB.viewProj = m_Camera.GetViewProjMatrix();

  pGfxContext->SetDynamicConstantBufferView(0, sizeof(cameraCB), &cameraCB);

  SystemGroup::OnUpdate(info);
}
void RenderSystem::OnPostUpdate(const UpdateInfo& info) {
  SystemGroup::OnPostUpdate(info);
  m_GC->End();
}

}  // namespace re::engine::render