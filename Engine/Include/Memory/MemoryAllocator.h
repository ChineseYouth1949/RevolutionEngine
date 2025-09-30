#pragma once

#include "Base/Base.h"

namespace RE::Core {

enum struct AllocType : uint8_t { STD = 0, MIMALLOC };

class RE_API MemoryAllocator {
 public:
  static void* Malloc(size_t size, AllocType type);
  static void* Zalloc(size_t size, AllocType type);
  static void* Calloc(size_t size, size_t count, AllocType type);

  static void* Recalloc(void* p, size_t newSize, AllocType type);
  static void* Recalloc(void* p, size_t newSize, size_t count, AllocType type);
  static bool Expand(void* p, size_t newSize, AllocType type);

  static void* MallocAligned(size_t size, size_t alignment, AllocType type);
  static void* ZallocAligned(size_t size, size_t alignment, AllocType type);
  static void* CallocAligned(size_t size, size_t count, size_t alignment, AllocType type);
  static void* ReallocAligned(void* p, size_t newsize, size_t alignment, AllocType type);

  static void* MallocAlignedAt(size_t size, size_t alignment, size_t offset, AllocType type);
  static void* ZallocAlignedAt(size_t size, size_t alignment, size_t offset, AllocType type);
  static void* CallocAlignedAt(size_t size, size_t count, size_t alignment, size_t offset, AllocType type);
  static void* ReallocAlignedAt(void* p, size_t newsize, size_t alignment, size_t offset, AllocType type);

  static void Free(void* p, AllocType type);
};

}  // namespace RE::Core