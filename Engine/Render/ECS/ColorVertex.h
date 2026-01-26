#pragma once

#include "Engine/Core/ECS/System.h"

namespace re::engine::render {
using namespace ecs;
class GraphicsCore;

struct ColorVertex {
  glm::vec3 position;
  glm::vec4 color;
};

class RenderColorVertex : public System {
 public:
  RenderColorVertex(GraphicsCore* gc);
  ~RenderColorVertex();

  bool Initialize() override;
  bool Release() override;

  void OnEnable() override;
  void OnDisable() override;

  void PreUpdate(const UpdateInfo& info) override;
  void Update(const UpdateInfo& info) override;
  void PostUpdate(const UpdateInfo& info) override;

 private:
  struct Impl;
  Alloc::UniquePtr<Impl> m_Impl{nullptr};
};

}  // namespace re::engine::render