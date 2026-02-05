#pragma once

#include "Pass.h"

#include "Engine/ECS/Resource/ResourceVisitor.h"
#include "Engine/ECS/World/RegistryVisitor.h"

namespace re::engine::ecs {
class World;

class RE_API SystemPass : public Pass {
 public:
  SystemPass(PassId id = PassIdFactory::Null, const string& name = "") : Pass(id, name) {}

  RE_FINLINE void Init(World* world) {
    m_ResourceVisitor = world->GetResourceManager()->RequestResourceVisitor();
    m_RegistryVisitor = world->RequestRegistryVisitor();
  }

 protected:
  ResourceVisitor m_ResourceVisitor;
  RegistryVisitor m_RegistryVisitor;
};
}  // namespace re::engine::ecs