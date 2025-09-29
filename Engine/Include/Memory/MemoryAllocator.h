#pragma once

#include "Base/Base.h"

namespace RE::Core {

enum struct AllocateType { STD = 0, Mimalloc };

class RE_API MemoryAllocator {
 public:
  MemoryAllocator(AllocateType type);
  virtual ~MemoryAllocator();

  // 基础分配
  void* Malloc(size_t size);
  void* Zalloc(size_t size);
  void* Calloc(size_t size, size_t count);

  // 调整分配
  void* Recalloc(void* p, size_t newSize);
  void* Recalloc(void* p, size_t newSize, size_t count);
  bool Expand(void* p, size_t newSize);

  // 对齐分配
  void* MallocAligned(size_t size, size_t alignment);
  void* ZallocAligned(size_t size, size_t alignment);
  void* CallocAligned(size_t size, size_t count, size_t alignment);
  void* ReallocAligned(void* p, size_t newsize, size_t alignment);

  // 带偏移的对齐分配
  void* MallocAlignedAt(size_t size, size_t alignment, size_t offset);
  void* ZallocAlignedAt(size_t size, size_t alignment, size_t offset);
  void* CallocAlignedAt(size_t size, size_t count, size_t alignment, size_t offset);
  void* ReallocAlignedAt(void* p, size_t newsize, size_t alignment, size_t offset);

  // Free memory allocate
  void Free(void* p);

  RE_INLINE static MemoryAllocator& GetDefaultAllocator() {
    static MemoryAllocator defaultAllocator(sDefaultType);
    return defaultAllocator;
  }

  template <typename T>
  RE_INLINE static T* DefaultNew(bool zero = true) {
    void* p = nullptr;
    if (zero) {
      p = GetDefaultAllocator().Zalloc(sizeof(T));
    } else {
      p = GetDefaultAllocator().Malloc(sizeof(T));
    }
    return static_cast<T*>(p);
  }

  template <typename T, typename... Args>
  RE_INLINE static T* DefaultNew(Args&&... args) {
    void* p = GetDefaultAllocator().Malloc(sizeof(T));
    return new (p) T(std::forward<Args>(args)...);
  }

  RE_INLINE static void DefaultDelete(void* p) { GetDefaultAllocator().Free(p); }

 private:
  RE_PIMPL

  static AllocateType sDefaultType;
};

}  // namespace RE::Core