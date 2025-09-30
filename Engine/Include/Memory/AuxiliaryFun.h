#pragma once

#include "MemoryAllocator.h"

namespace RE::Core {

template <typename T>
RE_INLINE T* Allocate(AllocType type) {
  void* p = nullptr;
  p = MemoryAllocator::Malloc(sizeof(T), type);
  return static_cast<T*>(p);
}

template <typename T>
RE_INLINE T* AllocateZero(AllocType type) {
  void* p = nullptr;
  p = MemoryAllocator::Zalloc(sizeof(T), type);
  return static_cast<T*>(p);
}

template <typename T>
RE_INLINE void Free(T* ptr, AllocType type) {
  void* p = static_cast<void*>(ptr);
  MemoryAllocator::Free(p, type);
}

template <typename T>
RE_INLINE void Dele(T* ptr, AllocType type) {
  ptr->~T();
  void* p = static_cast<void*>(ptr);
  MemoryAllocator::Free(p, type);
}

template <typename T, typename... Args>
RE_INLINE T* AllocateConstr(AllocType type, Args&&... args) {
  void* p = MemoryAllocator::Malloc(sizeof(T), type);
  return new (p) T(std::forward<Args>(args)...);
}

template <typename T, AllocType type>
struct MDeleter {
  void operator()(T* p) const noexcept {
    if (p) {
      p->~T();
      Free<T>(p, type);
    }
  }
};

template <typename T, AllocType type>
using MUniquePtr = std::unique_ptr<T, MDeleter<T, type>>;

}  // namespace RE::Core