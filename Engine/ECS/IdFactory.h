#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
RE_DEFINE_ID_FACTORY(ResIdFactory, 10000)
using ResouceId = ResIdFactory::ValueType;

using ComTypeIdFactory = utility::IdFactory;
using ComponentId = ComTypeIdFactory::ValueType;

using PassTypeIdFactory = utility::IdFactory;
using PassId = PassTypeIdFactory::ValueType;
}  // namespace re::engine::ecs