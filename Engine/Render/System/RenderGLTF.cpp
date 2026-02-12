#include "RenderGLTF.h"

#include "Engine/ECS/All.h"

namespace re::engine::render {
using namespace Microsoft::WRL;

using namespace re::engine::ecs;
using namespace re::engine;

struct RenderGLTF::Impl {
  GraphicsCore* m_GC;
  SharedInfo* m_SharedInfo;

  GraphicsPSO m_PSO;
  RootSignature m_RootSignature;

  struct ComResource {
    ByteAddressBuffer vertexBuffer;
    ByteAddressBuffer indexBuffer;
  };

  unordered_map<ecs::Entity, ComResource> m_EntityResources;
};

RenderGLTF::RenderGLTF() {}
RenderGLTF::~RenderGLTF() {}

void RenderGLTF::OnAttach(ecs::World* world) {
  ecs::System::OnAttach(world);

  auto resManger = world->GetResourceManager();

  auto gcResId = resManger->FindTypeResId<GraphicsCore>();
  RE_ASSERT(gcResId != ResId::Null);
  d()->m_GC = resManger->GetResource();
}

void RenderGLTF::OnDetach() {}

void RenderGLTF::OnEnable() {}
void RenderGLTF::OnDisable() {}

void RenderGLTF::OnPreUpdate() {}
void RenderGLTF::OnPostUpdate() {}

}  // namespace re::engine::render