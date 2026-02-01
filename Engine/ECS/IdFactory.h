#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
RE_DEFINE_ID_FACTORY(ResIdFactory)
using ResouceId = ResIdFactory::ValueType;

RE_DEFINE_ID_FACTORY(ComIdFactory)
using ComponentId = ComIdFactory::ValueType;

RE_DEFINE_ID_FACTORY(PassIdFactory)
using PassId = PassIdFactory::ValueType;
}  // namespace re::engine::ecs