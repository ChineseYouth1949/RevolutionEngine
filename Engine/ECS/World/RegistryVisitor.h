#pragma once

#include "RegistryVisitor.h"
#include "World.h"

namespace re::engine::ecs {
class RE_API RegistryVisitor {
 public:
  RegistryVisitor(World* world = nullptr) : m_World(world) {}

  RE_FINLINE void SetWorld(World* world) { m_World = world; }

  RE_FINLINE Registry* GetRegistry() {
    RE_ASSERT(m_World != nullptr);
    return m_World->GetRegistry();
  }

  RE_FINLINE void Submit() {
    RE_ASSERT(m_World != nullptr);
    m_World->Submit(std::move(m_RegistryCommandBuffer));
  }

 private:
  RegistryCommandBuffer m_RegistryCommandBuffer;
  World* m_World;
};
}  // namespace re::engine::ecs