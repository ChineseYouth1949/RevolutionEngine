#pragma once

#include "Engine/Base/All.h"
#include "Engine/ECS/All.h"

#include "GraphicsCore.h"
#include "Common.h"
#include "Mesh.h"

namespace re::engine::render {
struct GLTF {
  string filePath;
};

class RE_API RenderGLTF : public ecs::System {
 public:
  RenderGLTF();
  ~RenderGLTF();

  void OnAttach(ecs::World* world) override;
  void OnDetach() override;

  void OnEnable() override;
  void OnDisable() override;

  void OnPreUpdate() override;
  void OnPostUpdate() override;

 private:
  RE_DECLARE_PRIVATE
};

}  // namespace re::engine::render