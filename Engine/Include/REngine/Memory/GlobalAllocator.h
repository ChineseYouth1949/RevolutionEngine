#pragma once

#include "MemoryAllocator.h"

namespace RE::Engine {

const AllocType sGlobalAllocType = AllocType::STD;

using GMemoryAllocator = MemoryAllocator<sGlobalAllocType>;

template <typename T>
using GUniquePtr = GMemoryAllocator::UniquePtr<T>;

}  // namespace RE::Engine