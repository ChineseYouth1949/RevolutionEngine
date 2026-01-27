#include "Engine/Render/RHI/GraphicsCore.h"
#include "RenderSystem.h"

namespace re::engine::render {
RenderSystem::RenderSystem() {}
RenderSystem::~RenderSystem() {}

void RenderSystem::Init(shared_ptr<GraphicsCore> gc) {
  m_GC = gc;
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
  m_GC->Begin();
  SystemGroup::OnPreUpdate(info);
}
void RenderSystem::OnUpdate(const UpdateInfo& info) {
  SystemGroup::OnUpdate(info);
}
void RenderSystem::OnPostUpdate(const UpdateInfo& info) {
  SystemGroup::OnPostUpdate(info);
  m_GC->End();
}

}  // namespace re::engine::render