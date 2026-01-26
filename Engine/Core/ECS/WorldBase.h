#pragma once

#include "Engine/Core/Base.h"

namespace re::engine::ecs {
using EntityId = entt::entity;

class RE_API WorldBase {
 public:
  WorldBase();
  virtual ~WorldBase();

  RE_FINLE entt::registry& GetReg() { return m_Reg; }
  RE_FINLE void DestryEntity(EntityId id) { m_DestryEntitys.push_back(id); }

 protected:
  entt::registry m_Reg;
  std::vector<EntityId> m_DestryEntitys;
};
}  // namespace re::engine::ecs