#pragma once

#include "Stage.h"

#include "Engine/ECS/Resource/ResourceCommandBuffer.h"
#include "Engine/ECS/World/WorldCommandBuffer.h"

namespace re::engine::ecs {
class World;

#define RE_DEFINE_STAGE(Name) \
  class Name {};

RE_DEFINE_STAGE(WorldProUpdate)
RE_DEFINE_STAGE(WorldUpdate)
RE_DEFINE_STAGE(WorldPostUpdate)

class RE_API WorldStage : public Stage {
 public:
  WorldStage();

  RE_FINLINE void SetWolrd(World* world) { m_World = world; }

 private:
  World* m_World;

  ResCommandBuffer m_ResCommandBuffer;
  WorldCommandBuffer m_WorldCommandBuffer;
};
}  // namespace re::engine::ecs