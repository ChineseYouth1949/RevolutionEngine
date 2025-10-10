#pragma once

#include "MemoryAllocator.h"
#include "AuxiliaryFun.h"

namespace RE::Core {

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

template <typename T>
RE_INLINE void GDele(T* ptr) {
  Dele<T>(ptr, globalAllocType);
}

template <typename T, typename... Args>
RE_INLINE T* GAllocateConstr(Args&&... args) {
  return AllocateConstr<T>(globalAllocType, std::forward<Args>(args)...);
}

template <typename T>
using GDeleter = MDeleter<T, globalAllocType>;

template <typename T>
using GUniquePtr = MUniquePtr<T, globalAllocType>;

template <typename T, typename... Args>
RE_INLINE GUniquePtr<T> GAllocateConstrUnique(Args&&... args) {
  GUniquePtr<T> res = GUniquePtr<T>(AllocateConstr<T>(globalAllocType, std::forward<Args>(args)...));
  return std::move(res);
}

}  // namespace RE::Core