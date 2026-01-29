#pragma once

#include "Engine/Base/All.h"
#include "Engine/ECS/All.h"

#include "GraphicsCore.h"
#include "Common.h"
#include "Mesh.h"

namespace re::engine::render {
class RenderAxis : public ecs::System {
 public:
  RenderAxis();
  ~RenderAxis();

  void Init(shared_ptr<GraphicsCore> gc, shared_ptr<SharedInfo> si);

  bool OnAttach() override;
  bool OnDetach() override;

  void OnEnable() override;
  void OnDisable() override;

  void OnPreUpdate(const ecs::UpdateInfo& info) override;
  void OnUpdate(const ecs::UpdateInfo& info) override;
  void OnPostUpdate(const ecs::UpdateInfo& info) override;

 private:
  shared_ptr<GraphicsCore> m_GC;
  shared_ptr<SharedInfo> m_SharedInfo;

  GraphicsPSO m_PSO;
  RootSignature m_RootSignature;

  ByteAddressBuffer m_VertexBuffer;

  struct Vertex {
    glm::vec3 position;
  };

  struct ConstBuffer {
    glm::vec4 backgroundColor;
    glm::vec4 gridColor;
    glm::vec3 cameraPos;
    float _padding1;
    int32_t screenResolution[2];

    float gridSize;
    float lineWidth;
    float lineSoftness;
  };
  ConstBuffer m_ConstBuffer;

  void UpdateCB();
  void Render();
};

}  // namespace re::engine::render