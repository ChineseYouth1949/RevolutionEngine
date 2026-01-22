#pragma once

#include "Engine/Core/Base.h"

namespace re::core::ecs {
using EntityId = entt::entity;

class RE_API WorldBase {
 public:
  WorldBase();
  virtual ~WorldBase();

  RE_FINLE entt::registry& GetReg() { return m_Reg; }
  RE_FINLE void DestryEntity(EntityId id) { m_DestryEntitys.push_back(id); }
  RE_FINLE const std::vector<EntityId>& GetDestryEntitys() const { return m_DestryEntitys; }

  RE_FINLE void NextFrame() {
    for (auto de : m_DestryEntitys) {
      m_Reg.destroy(de);
    }

    m_DestryEntitys.clear();
  }

 protected:
  entt::registry m_Reg;
  std::vector<EntityId> m_DestryEntitys;
};
}  // namespace re::core::ecs