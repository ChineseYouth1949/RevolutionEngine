#pragma once

#include "Engine/Core/Core.h"

#include "GraphicsCore.h"
#include "Engine/Render/RHI/Camera.h"

namespace re::engine::render {

class RE_API RenderSystem : public ecs::SystemGroup {
 public:
  RenderSystem();
  ~RenderSystem();

  void Init(shared_ptr<GraphicsCore> gc);

  bool OnAttach() override;
  bool OnDetach() override;

  void OnEnable() override;
  void OnDisable() override;

  void OnPreUpdate(const ecs::UpdateInfo& info) override;
  void OnUpdate(const ecs::UpdateInfo& info) override;
  void OnPostUpdate(const ecs::UpdateInfo& info) override;

 private:
  shared_ptr<GraphicsCore> m_GC;
  Math::Camera m_Camera;
};
}  // namespace re::engine::render