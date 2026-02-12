#pragma once

#include "Engine/Memory/Allocator.h"

namespace re::engine::type {
const memory::AllocType GlobalAllocType = memory::AllocType::MiMalloc;
using GAlloc = memory::Allocator<GlobalAllocType>;
}  // namespace re::engine::type