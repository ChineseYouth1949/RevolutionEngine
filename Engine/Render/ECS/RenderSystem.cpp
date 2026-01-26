#include "Engine/Render/RHI/GraphicsCore.h"
#include "RenderSystem.h"

#include "Mesh.h"

using namespace re::engine::render;
using namespace re::engine::ecs;

struct RenderSystem::Impl {
  GraphicsCore gc;
};

RenderSystem::RenderSystem() {}
RenderSystem::~RenderSystem() {}

bool RenderSystem::Initialize() {
  m_Impl = Alloc::CreateUniquePtr<Impl>();

  GCInitInfo info;
  m_Impl->gc.Initialize(info);

  return true;
}
bool RenderSystem::Release() {
  m_Impl = nullptr;
  return true;
}

void RenderSystem::OnEnable() {}
void RenderSystem::OnDisable() {}

void RenderSystem::PreUpdate(const UpdateInfo& info) {}
void RenderSystem::Update(const UpdateInfo& info) {}
void RenderSystem::PostUpdate(const UpdateInfo& info) {}