#pragma once

#include "Container.h"

namespace re::engine::continer {
template <typename T, typename... Args>
RE_FINLINE static shared_ptr<T> make_shared(Args&&... args) {
  return allocate_shared<T, StlAllocator<type, T>>(StlAllocator<type, T>(), std::forward<Args>(args)...);
}

template <typename T, typename... Args>
RE_FINLINE static unique_ptr<T, Deleter<T>> make_unique(Args&&... args) {
  T* ptr = create<T>(stl::forward<Args>(args)...);
  return unique_ptr<T, Deleter<T>>(ptr);
}

}  // namespace re::engine::continer