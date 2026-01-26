#pragma once

#include "Allocator.h"

namespace re::engine::memory {

template <typename T>
class StlAllocator<AllocType::STD, T> : public std::allocator<T> {
 public:
  using std::allocator<T>::allocator;

  StlAllocator(const char* pName) noexcept : std::allocator<T>() { (void)pName; }

  template <typename U>
  struct rebind {
    using other = StlAllocator<AllocType::STD, U>;
  };
};

template <typename T>
class StlAllocator<AllocType::MiMalloc, T> : public mi_stl_allocator<T> {
 public:
  using mi_stl_allocator<T>::mi_stl_allocator;

  StlAllocator(const char* pName) noexcept : mi_stl_allocator<T>() { (void)pName; }

  T* allocate(size_t n) { return mi_stl_allocator<T>::allocate(n); }
  T* allocate(size_t n, int flags, const char* pName = nullptr) {
    (void)flags;
    (void)pName;
    return mi_stl_allocator<T>::allocate(n);
  }
  T* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) {
    (void)offset;
    (void)flags;
    return static_cast<T*>(mi_malloc_aligned(n * sizeof(T), alignment));
  }

  void deallocate(void* p, size_t n) noexcept {
    mi_free(p);  // 直接调用 mimalloc 释放
  }
  void deallocate(T* p, size_t n) noexcept { mi_free(p); }

  template <typename U>
  struct rebind {
    using other = StlAllocator<AllocType::MiMalloc, U>;
  };
};

}  // namespace re::engine::memory