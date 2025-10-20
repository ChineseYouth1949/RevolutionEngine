#pragma once

#include "MemoryAllocator.h"
#include "Auxiliary.h"

namespace RE::Engine {

const AllocType globalAllocType = AllocType::STD;

RE_INLINE void* GMalloc(size_t size) {
  return MemoryAllocator::Malloc(size, globalAllocType);
}
RE_INLINE void* GCalloc(size_t count, size_t size) {
  return MemoryAllocator::Calloc(size, count, globalAllocType);
}
RE_INLINE void* GRealloc(void* p, size_t size) {
  return MemoryAllocator::Realloc(p, size, globalAllocType);
}

RE_INLINE void GFree(void* p) {
  MemoryAllocator::Free(p, globalAllocType);
}

template <typename T>
RE_INLINE T* GAllocate() {
  return Allocate<T>(globalAllocType);
}

template <typename T>
RE_INLINE T* GAllocateZero() {
  return AllocateZero<T>(globalAllocType);
}

template <typename T>
RE_INLINE void GFree(T* ptr) {
  Free<T>(ptr, globalAllocType);
}

template <typename T, typename... Args>
RE_INLINE T* GAllocateConstructor(Args&&... args) {
  return AllocateConstructor<T>(globalAllocType, std::forward<Args>(args)...);
}

template <typename T>
RE_INLINE void GDestry(T* ptr) {
  Destry<T>(ptr, globalAllocType);
}

template <typename T>
using GDeleter = Deleter<T, globalAllocType>;

template <typename T>
using GUniquePtr = UniquePtr<T, globalAllocType>;

}  // namespace RE::Engine