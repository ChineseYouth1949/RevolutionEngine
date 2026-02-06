#pragma once

#include "Engine/Base/All.h"
#include "Engine/ECS/All.h"

#include "GraphicsCore.h"
#include "Common.h"

namespace re::engine::render {

class RE_API RenderSystem : public ecs::SystemGroup {
 public:
  RenderSystem();
  ~RenderSystem();

  void Init(engine::shared_ptr<GraphicsCore> gc);

  void OnAttach(ecs::World* world) override;
  void OnDetach() override;

  void OnEnable() override;
  void OnDisable() override;

  void OnPreUpdate() override;
  void OnPostUpdate() override;

  engine::shared_ptr<SharedInfo> GetSharedInfo() { return m_SharedInfo; }

 private:
  engine::shared_ptr<GraphicsCore> m_GC;
  engine::shared_ptr<SharedInfo> m_SharedInfo;
};
}  // namespace re::engine::render