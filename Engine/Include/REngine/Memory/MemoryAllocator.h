#pragma once

#include <memory>

#include "Malloc.h"

namespace RE::Engine {

#define COPY_ENGINE_MEMORY_FUN(funName)

template <AllocType type>
class RE_DECLSPEC MemoryAllocator {
 public:
  // class obj function
  template <typename T>
  RE_INLINE static T* Allocate() {
    void* p = nullptr;
    p = MemoryAllocator::Malloc(sizeof(T));
    return static_cast<T*>(p);
  }

  template <typename T>
  RE_INLINE static T* AllocateZero() {
    void* p = nullptr;
    p = MemoryAllocator::Zalloc(sizeof(T));
    return static_cast<T*>(p);
  }

  template <typename T, typename... Args>
  RE_INLINE static T* Create(Args&&... args) {
    void* p = MemoryAllocator::Malloc(sizeof(T));
    return new (p) T(std::forward<Args>(args)...);
  }

  template <typename T>
  RE_INLINE static void Destry(T* ptr) {
    ptr->~T();
    void* p = static_cast<void*>(ptr);
    MemoryAllocator::Free(p);
  }

  template <typename T>
  struct Deleter {
    void operator()(T* p) const noexcept {
      if (p) {
        p->~T();
        MemoryAllocator::Free(p);
      }
    }
  };

  template <typename T>
  using UniquePtr = std::unique_ptr<T, Deleter<T>>;

  template <typename T, typename... Args>
  RE_INLINE static UniquePtr<T> CreateUniquePtr(Args&&... args) {
    T* ptr = Create<T>(std::forward<Args>(args)...);
    return UniquePtr<T>(ptr);
  }

  // malloc memory function
  RE_INLINE static void* Malloc(size_t size) { return Engine::Malloc(size, type); }
  RE_INLINE static void* Zalloc(size_t size) { return Engine::Zalloc(size, type); }
  RE_INLINE static void* Calloc(size_t size, size_t count) { return Engine::Calloc(size, count, type); }

  RE_INLINE static void* Realloc(void* p, size_t newSize) { return Engine::Realloc(p, newSize, type); }
  RE_INLINE static void* ReallocN(void* p, size_t newSize, size_t count) { return Engine::ReallocN(p, newSize, count, type); }
  RE_INLINE static bool Expand(void* p, size_t newSize) { return Engine::Expand(p, newSize, type); }

  RE_INLINE static void* MallocAligned(size_t size, size_t alignment) { return Engine::MallocAligned(size, alignment, type); }
  RE_INLINE static void* ZallocAligned(size_t size, size_t alignment) { return Engine::ZallocAligned(size, alignment, type); }
  RE_INLINE static void* CallocAligned(size_t size, size_t count, size_t alignment) { return Engine::CallocAligned(size, count, type); }
  RE_INLINE static void* ReallocAligned(void* p, size_t newsize, size_t alignment) { return Engine::ReallocAligned(p, newsize, alignment, type); }

  RE_INLINE static void* MallocAlignedAt(size_t size, size_t alignment, size_t offset) {
    return Engine::MallocAlignedAt(size, alignment, offset, type);
  }
  RE_INLINE static void* ZallocAlignedAt(size_t size, size_t alignment, size_t offset) {
    return Engine::ZallocAlignedAt(size, alignment, offset, type);
  }
  RE_INLINE static void* CallocAlignedAt(size_t size, size_t count, size_t alignment, size_t offset) {
    return Engine::CallocAlignedAt(size, count, alignment, offset, type);
  }
  RE_INLINE static void* ReallocAlignedAt(void* p, size_t newsize, size_t alignment, size_t offset) {
    return Engine::ReallocAlignedAt(p, newsize, alignment, offset, type);
  }

  RE_INLINE static void Free(void* p) { return Engine::Free(p, type); }
};

}  // namespace RE::Engine