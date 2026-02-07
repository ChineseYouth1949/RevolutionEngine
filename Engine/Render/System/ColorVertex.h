#pragma once

#include "Engine/Base/All.h"
#include "Engine/ECS/All.h"

#include "GraphicsCore.h"
#include "Common.h"
#include "Mesh.h"

namespace re::engine::render {
struct ColorVertex {
  glm::vec3 position;
  glm::vec4 color;
};

class RenderColorVertex : public ecs::System {
 public:
  using SysComType = Mesh<ColorVertex, uint32_t>;

  RenderColorVertex();
  ~RenderColorVertex();

  void Init(engine::shared_ptr<GraphicsCore> gc, engine::shared_ptr<SharedInfo> si);

  void OnAttach(ecs::World* world) override;
  void OnDetach() override;

  void OnEnable() override;
  void OnDisable() override;

  void OnPreUpdate() override;
  void OnPostUpdate() override;

 private:
  engine::shared_ptr<GraphicsCore> m_GC;
  engine::shared_ptr<SharedInfo> m_SharedInfo;

  GraphicsPSO m_PSO;
  RootSignature m_RootSignature;

  void PollAddComponent();
  void PollRemoveComponent();
  void PollChangeComponent();

  ByteAddressBuffer m_DefaultVertexBuffer;
  ByteAddressBuffer m_DefaultIndexBuffer;

  struct Resource {
    ByteAddressBuffer vertexBuffer;
    ByteAddressBuffer indexBuffer;
  };
  Resource GetResource(SysComType& com);
  void Render();
  unordered_map<ecs::Entity, Resource> m_EntityResources;
};

}  // namespace re::engine::render