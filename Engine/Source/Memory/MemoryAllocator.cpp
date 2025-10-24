#include "Memory/Malloc.h"

#include <cstdlib>
#include <memory>

#include <mimalloc.h>

namespace RE::Engine {

void* Malloc(size_t size, AllocType type) {
  if (size == 0) {
    return nullptr;
  }

  void* res = nullptr;

  switch (type) {
    case AllocType::STD:
      res = std::malloc(size);
      break;
    case AllocType::MIMALLOC:
      res = mi_malloc(size);
    default:
      break;
  }

  return res;
}

void* Zalloc(size_t size, AllocType type) {
  if (size == 0) {
    return nullptr;
  }

  void* res = nullptr;

  switch (type) {
    case AllocType::STD:
      res = std::malloc(size);
      memset(res, 0, size);
      break;
    case AllocType::MIMALLOC:
      res = mi_zalloc(size);
    default:
      break;
  }

  return res;
}

void* Calloc(size_t size, size_t count, AllocType type) {
  return Zalloc(size * count, type);
}

void* Realloc(void* p, size_t newSize, AllocType type) {
  if (p == nullptr) {
    return Malloc(newSize, type);
  }

  void* res = nullptr;

  switch (type) {
    case AllocType::STD:
      res = std::realloc(p, newSize);
      break;
    case AllocType::MIMALLOC:
      res = mi_realloc(res, newSize);
    default:
      break;
  }

  return res;
}

void* ReallocN(void* p, size_t newSize, size_t count, AllocType type) {
  return Realloc(p, newSize * count, type);
}

bool Expand(void* p, size_t newSize, AllocType type) {
  if (!p) {
    return false;
  }

  void* res = nullptr;

  switch (type) {
    case AllocType::STD:
      break;
    case AllocType::MIMALLOC:
      res = mi_expand(p, newSize);
    default:
      break;
  }

  return res == p;
}

RE_INLINE void* AlignedAlloc(size_t size, size_t align) {
#if defined(_WIN32)
  return _aligned_malloc(size, align);
#elif defined(__linux__)
  return ::aligned_alloc(align, size);
#else
  return std::aligned_alloc(align, size);
#endif
}

void* MallocAligned(size_t size, size_t alignment, AllocType type) {
  if (alignment == 0) {
    alignment = alignof(std::max_align_t);
  }

  void* res = nullptr;

  switch (type) {
    case AllocType::STD:
      res = AlignedAlloc(size, alignment);
      break;
    case AllocType::MIMALLOC:
      res = mi_malloc_aligned(size, alignment);
    default:
      break;
  }

  return res;
}

void* ZallocAligned(size_t size, size_t alignment, AllocType type) {
  if (alignment == 0) {
    alignment = alignof(std::max_align_t);
  }

  void* res = nullptr;

  switch (type) {
    case AllocType::STD:
      res = AlignedAlloc(size, alignment);
      memset(res, 0, size);
      break;
    case AllocType::MIMALLOC:
      res = mi_zalloc_aligned(size, alignment);
    default:
      break;
  }

  return res;
}

void* CallocAligned(size_t size, size_t count, size_t alignment, AllocType type) {
  return ZallocAligned(size * count, alignment, type);
}

void* ReallocAligned(void* p, size_t newsize, size_t alignment, AllocType type) {
  if (p) {
    Free(p, type);
  }

  return MallocAligned(newsize, alignment, type);
}

void* MallocAlignedAt(size_t size, size_t alignment, size_t offset, AllocType type) {
  if (offset >= alignment) {
    return nullptr;
  }

  void* res = nullptr;
  uintptr_t aligned_ptr;

  switch (type) {
    case AllocType::STD:
      res = MallocAligned(size + alignment, alignment, type);
      aligned_ptr = reinterpret_cast<uintptr_t>(res);
      aligned_ptr = (aligned_ptr + offset + alignment - 1) & ~(alignment - 1);
      res = reinterpret_cast<void*>(aligned_ptr);
      break;
    case AllocType::MIMALLOC:
      res = mi_malloc_aligned_at(size, alignment, offset);
    default:
      break;
  }

  return res;
}

void* ZallocAlignedAt(size_t size, size_t alignment, size_t offset, AllocType type) {
  if (offset >= alignment) {
    return nullptr;
  }

  void* res = nullptr;
  uintptr_t aligned_ptr;

  switch (type) {
    case AllocType::STD:
      res = ZallocAligned(size + alignment, alignment, type);
      aligned_ptr = reinterpret_cast<uintptr_t>(res);
      aligned_ptr = (aligned_ptr + offset + alignment - 1) & ~(alignment - 1);
      res = reinterpret_cast<void*>(aligned_ptr);
      break;
    case AllocType::MIMALLOC:
      res = mi_malloc_aligned_at(size, alignment, offset);
    default:
      break;
  }

  return res;
}

void* CallocAlignedAt(size_t size, size_t count, size_t alignment, size_t offset, AllocType type) {
  return ZallocAlignedAt(size * count, alignment, offset, type);
}

void* ReallocAlignedAt(void* p, size_t newsize, size_t alignment, size_t offset, AllocType type) {
  if (p) {
    Free(p, type);
  }

  return MallocAlignedAt(newsize, alignment, offset, type);
}

void Free(void* p, AllocType type) {
  if (p) {
    switch (type) {
      case AllocType::STD:
        free(p);
        break;
      case AllocType::MIMALLOC:
        mi_free(p);
      default:
        break;
    }
  }
}

}  // namespace RE::Engine