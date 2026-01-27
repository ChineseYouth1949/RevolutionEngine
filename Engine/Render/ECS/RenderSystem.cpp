#include "Engine/Render/RHI/GraphicsCore.h"
#include "RenderSystem.h"

namespace re::engine::render {

struct RenderSystem::Impl {
  GraphicsCore gc;
};

RenderSystem::RenderSystem() {}
RenderSystem::~RenderSystem() {}

bool RenderSystem::OnAttach() {
  m_Impl = GAlloc::make_unique<Impl>();

  GCInitInfo info;
  m_Impl->gc.Initialize(info);

  SystemGroup::OnAttach();

  return true;
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
  SystemGroup::OnUpdate(info);
}
void RenderSystem::OnPostUpdate(const UpdateInfo& info) {
  SystemGroup::OnPostUpdate(info);
}

}  // namespace re::engine::render