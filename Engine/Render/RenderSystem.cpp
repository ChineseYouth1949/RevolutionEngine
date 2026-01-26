#include "RenderSystem.h"

using namespace re::engine::render;
using namespace re::engine::ecs;

RenderSystem::RenderSystem() {}
RenderSystem::~RenderSystem() {}

bool RenderSystem::Initialize() {
  m_GC = Alloc::CreateUniquePtr<GraphicsCore>();

  GCInitInfo info;
  m_GC->Initialize(info);

  return true;
}
bool RenderSystem::Release() {
  m_GC = nullptr;
  return true;
}

void RenderSystem::OnEnable() {}
void RenderSystem::OnDisable() {}

void RenderSystem::PreUpdate(const UpdateInfo& info) {}
void RenderSystem::Update(const UpdateInfo& info) {}
void RenderSystem::PostUpdate(const UpdateInfo& info) {}