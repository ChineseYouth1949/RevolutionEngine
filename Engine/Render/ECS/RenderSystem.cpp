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

  SystemGroup::Initialize();

  return true;
}
bool RenderSystem::Release() {
  SystemGroup::Release();
  m_Impl = nullptr;
  return true;
}

void RenderSystem::OnEnable() {
  SystemGroup::OnEnable();
}
void RenderSystem::OnDisable() {
  SystemGroup::OnDisable();
}

void RenderSystem::PreUpdate(const UpdateInfo& info) {
  SystemGroup::PreUpdate(info);
}
void RenderSystem::Update(const UpdateInfo& info) {
  SystemGroup::Update(info);
}
void RenderSystem::PostUpdate(const UpdateInfo& info) {
  SystemGroup::PostUpdate(info);
}