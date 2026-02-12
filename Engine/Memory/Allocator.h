#pragma once

#include "Engine/Type/Common.h"
#include "Engine/Macros.h"

#define RE_MALLOC_ALIGNMENT 16

namespace re::engine::memory {

enum struct AllocType : uint8 { STD, MiMalloc };

// Support STL and MiMalloc
template <AllocType type, typename T>
class StlAllocator;

/*
*  Allocator not supported AllocType::STD, because MSVC cannot implement std::align_malloc, 
*  and Windows memory allocation requires the separate use of free and _aligned_free to release memory, 
*  which does not conform to the unified free interface. 
*  In short, using mimalloc to implement the Allocator performs well enough.
*/
template <AllocType type>
class RE_API Allocator {
 public:
  // No type alloc
  RE_FINLINE static void* malloc(size_t size) noexcept;
  RE_FINLINE static void* zalloc(size_t size) noexcept;
  RE_FINLINE static void* calloc(size_t count, size_t size) noexcept;

  RE_FINLINE static void* realloc(void* p, size_t newSize) noexcept;
  RE_FINLINE static void* reallocN(void* p, size_t count, size_t newSize) noexcept;
  RE_FINLINE static bool expand(void* p, size_t newSize) noexcept;

  RE_FINLINE static void* mallocAligned(size_t size, size_t alignment) noexcept;
  RE_FINLINE static void* zallocAligned(size_t size, size_t alignment) noexcept;
  RE_FINLINE static void* callocAligned(size_t count, size_t size, size_t alignment) noexcept;
  RE_FINLINE static void* reallocAligned(void* p, size_t newsize, size_t alignment) noexcept;

  RE_FINLINE static void* mallocAlignedAt(size_t size, size_t alignment, size_t offset) noexcept;
  RE_FINLINE static void* zallocAlignedAt(size_t size, size_t alignment, size_t offset) noexcept;
  RE_FINLINE static void* callocAlignedAt(size_t count, size_t size, size_t alignment, size_t offset) noexcept;
  RE_FINLINE static void* reallocAlignedAt(void* p, size_t newsize, size_t alignment, size_t offset) noexcept;

  RE_FINLINE static void free(void* p) noexcept;

  // Type alloc
  template <typename T>
  RE_FINLINE static T* mallocType() noexcept {
    void* p = nullptr;
    p = malloc(sizeof(T));
    return static_cast<T*>(p);
  }

  template <typename T>
  RE_FINLINE static T* mallocTypeArray(size_t count) noexcept {
    size_t alignment = alignof(T);
    void* p = (alignment > RE_MALLOC_ALIGNMENT) ? mallocAligned(count * sizeof(T), alignment) : malloc(count * sizeof(T));
    return static_cast<T*>(p);
  }

  template <typename T>
  RE_FINLINE static T* zallocType() noexcept {
    void* p = nullptr;
    p = zalloc(sizeof(T));
    return static_cast<T*>(p);
  }

  template <typename T>
  RE_FINLINE static T* zallocTypeArray(size_t count) noexcept {
    size_t alignment = alignof(T);
    void* p = (alignment > RE_MALLOC_ALIGNMENT) ? zallocAligned(count * sizeof(T), alignment) : zalloc(count * sizeof(T));
    return static_cast<T*>(p);
  }

  template <typename T, typename... Args>
  RE_FINLINE static T* create(Args&&... args) {
    size_t alignment = alignof(T);
    void* p = (alignment > RE_MALLOC_ALIGNMENT) ? mallocAligned(sizeof(T), alignment) : malloc(sizeof(T));
    if (!p)
      return nullptr;
    return new (p) T(std::forward<Args>(args)...);
  }

  template <typename T, typename... Args>
  RE_FINLINE static T* createArray(size_t count, Args&&... args) {
    T* p = mallocTypeArray<T>(count);
    if (!p)
      return nullptr;

    // Construct objects with exception-safety: if a constructor throws,
    // destruct previously constructed objects and free memory, then rethrow.
    size_t constructed = 0;
    try {
      for (size_t i = 0; i < count; ++i) {
        new (&p[i]) T(std::forward<Args>(args)...);
        ++constructed;
      }
      return p;
    } catch (...) {
      for (size_t i = 0; i < constructed; ++i) {
        p[i].~T();
      }
      free(static_cast<void*>(p));
      throw;
    }
  }

  template <typename T>
  RE_FINLINE static void destroy(T* ptr) noexcept {
    if (!ptr) {
      return;
    }

    ptr->~T();
    void* p = static_cast<void*>(ptr);
    free(p);
  }

  template <typename T>
  RE_FINLINE static void destroyArray(T* ptr, size_t count) noexcept {
    if (!ptr)
      return;

    // Destruct in reverse order (standard C++ behavior)
    for (size_t i = count; i > 0; --i) {
      ptr[i - 1].~T();
    }
    free(static_cast<void*>(ptr));
  }

  template <typename T>
  struct Deleter {
    void operator()(T* p) const noexcept { destroy<T>(p); }
  };

  template <template <typename> typename SharedPtr, typename T>
  using temp_shared_ptr = SharedPtr<T>;

  template <template <typename, typename> typename UniquePtr, typename T>
  using temp_unique_ptr = UniquePtr<T, Deleter<T>>;
};

}  // namespace re::engine::memory

// Convenient macro
#define RE_CLASS_ALLOCATOR(Type) using Alloc = re::engine::memory::Allocator<re::engine::memory::AllocType::Type>;
#define RE_NAME_ALLOCATOR(Name, Type) using Name = re::engine::memory::Allocator<re::engine::memory::AllocType::Type>

#include "Mimalloc.h"
#include "StlAllocator.h"
#include "OverNewDelete.h"