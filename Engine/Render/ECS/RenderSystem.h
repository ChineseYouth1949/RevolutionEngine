#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Render/RHI/GraphicsCore.h"

namespace re::engine::render {
using namespace ecs;

class RE_API RenderSystem : public SystemGroup {
 public:
  RenderSystem();
  ~RenderSystem();

  void Init(shared_ptr<GraphicsCore> gc);

  bool OnAttach() override;
  bool OnDetach() override;

  void OnEnable() override;
  void OnDisable() override;

  void OnPreUpdate(const UpdateInfo& info) override;
  void OnUpdate(const UpdateInfo& info) override;
  void OnPostUpdate(const UpdateInfo& info) override;

 private:
  shared_ptr<GraphicsCore> m_GC;
};
}  // namespace re::engine::render