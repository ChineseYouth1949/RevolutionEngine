#pragma once

#include "Engine/Core/ECS/System.h"
#include "GraphicsCore.h"

namespace re::engine::render {
using namespace ecs;

class RenderSystem : public System {
 public:
  RenderSystem();
  ~RenderSystem();

  bool Initialize() override;
  bool Release() override;

  void OnEnable() override;
  void OnDisable() override;

  void PreUpdate(const UpdateInfo& info) override;
  void Update(const UpdateInfo& info) override;
  void PostUpdate(const UpdateInfo& info) override;

 private:
  Alloc::UniquePtr<GraphicsCore> m_GC{nullptr};
};
}  // namespace re::engine::render