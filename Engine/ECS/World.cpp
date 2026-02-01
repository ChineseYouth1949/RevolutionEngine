#pragma once

#include "World.h"

namespace re::engine::ecs {
World::World() {
  m_ResourceManager = GAlloc::make_unique<ResourceManager>();
}
World::~World() {}
}  // namespace re::engine::ecs