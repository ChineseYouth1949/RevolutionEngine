#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
RE_DEFINE_ID_FACTORY(ResIdFactory, 10000)
using ResouceId = ResIdFactory::ValueType;

using ComIdFactory = utility::IdFactory;
using ComponentId = ComIdFactory::ValueType;

using PassIdFactory = utility::IdFactory;
using PassId = PassIdFactory::ValueType;
}  // namespace re::engine::ecs