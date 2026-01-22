#pragma once

#include "Engine/Core/Base.h"

#include "WorldBase.h"
#include "Entity.h"

namespace re::core::ecs {
class RE_API World : public WorldBase {
 public:
  World();
  ~World();

  RE_FINLE Entity CreateEntity() { return Entity(m_Reg.create(), this); }
};
}  // namespace re::core::ecs