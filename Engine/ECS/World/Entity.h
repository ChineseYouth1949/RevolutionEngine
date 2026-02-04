#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
using Entity = entt::entity;
using Registry = entt::basic_registry<Entity, GAllocSTL<Entity>>;
}  // namespace re::engine::ecs