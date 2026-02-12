#pragma once

#include "Engine/Macros.h"
#include "Engine/Memory/Allocator.h"

#include "STL.h"

namespace re::engine::type {
constexpr memory::AllocType PointerAllocType = memory::AllocType::MiMalloc;
using PointerAlloc = memory::Allocator<PointerAllocType>;

template <typename T>
using PointerAllocSTL = memory::StlAllocator<PointerAllocType, T>;

template <typename T>
using shared_ptr = PointerAlloc::temp_shared_ptr<stl::shared_ptr, T>;

template <typename T>
using unique_ptr = PointerAlloc::temp_unique_ptr<stl::unique_ptr, T>;

template <typename T, typename... Args>
RE_FINLINE static shared_ptr<T> make_shared(Args&&... args) {
  return stl::allocate_shared<T, PointerAllocSTL<T>>(PointerAllocSTL<T>(), std::forward<Args>(args)...);
}

template <typename T, typename... Args>
RE_FINLINE static unique_ptr<T, PointerAlloc::Deleter<T>> make_unique(Args&&... args) {
  T* ptr = PointerAlloc::create<T>(stl::forward<Args>(args)...);
  return unique_ptr<T, PointerAlloc::Deleter<T>>(ptr);
}

}  // namespace re::engine::type