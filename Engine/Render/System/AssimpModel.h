#pragma once

#include "Engine/Base/All.h"
#include "Engine/ECS/All.h"

#include "GraphicsCore.h"
#include "Common.h"
#include "Mesh.h"

// Optional Assimp support. Define RE_USE_ASSIMP in your build to enable.
#ifdef RE_USE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

namespace re::engine::render {

struct PBRVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec2 uv;
};

class RenderAssimpModel : public ecs::System {
 public:
  using SysComType = Mesh<PBRVertex, uint32_t>;

  RenderAssimpModel();
  ~RenderAssimpModel();

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

  struct Resource {
    ByteAddressBuffer vertexBuffer;
    ByteAddressBuffer indexBuffer;
  };

  Resource GetResource(SysComType& com);
  void Render();

  unordered_map<ecs::Entity, Resource> m_EntityResources;

#ifdef RE_USE_ASSIMP
  bool LoadModelFromFile(const wstring& path, SysComType& out);
#endif
  void CreateDefaultCube(SysComType& out);
};

}  // namespace re::engine::render
