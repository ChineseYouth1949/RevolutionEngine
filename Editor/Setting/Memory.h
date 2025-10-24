#pragma once

#include <REngine/Memory/Memory.h>

namespace RE::Editor {

const Engine::AllocType sGlobalMemoryAllocType = Engine::AllocType::STD;

using GMemoryAllocator = Engine::MemoryAllocator<sGlobalMemoryAllocType>;

template <typename T>
using GUniquePtr = GMemoryAllocator::UniquePtr<T>;

}  // namespace RE::Editor