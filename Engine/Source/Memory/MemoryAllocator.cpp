#include "Memory/MemoryAllocator.h"

#include <cstdlib>
#include <memory>

#include <mimalloc.h>

namespace RE::Core {

void* MemoryAllocator::Malloc(size_t size, AllocType type) {
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

void* MemoryAllocator::Zalloc(size_t size, AllocType type) {
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

void* MemoryAllocator::Calloc(size_t size, size_t count, AllocType type) {
  return Zalloc(size * count, type);
}

void* MemoryAllocator::Recalloc(void* p, size_t newSize, AllocType type) {
  if (!p) {
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

void* MemoryAllocator::Recalloc(void* p, size_t newSize, size_t count, AllocType type) {
  return Recalloc(p, newSize * count, type);
}

bool MemoryAllocator::Expand(void* p, size_t newSize, AllocType type) {
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

RE_INLINE void* AlignedAlloc(size_t align, size_t size) {
#if defined(_WIN32)
  return _aligned_malloc(size, align);
#elif defined(__linux__)
  return ::aligned_alloc(align, size);
#else
  return std::aligned_alloc(align, size);
#endif
}

void* MemoryAllocator::MallocAligned(size_t size, size_t alignment, AllocType type) {
  if (alignment == 0) {
    alignment = alignof(std::max_align_t);
  }

  void* res = nullptr;

  switch (type) {
    case AllocType::STD:
      res = AlignedAlloc(alignment, size);
      break;
    case AllocType::MIMALLOC:
      res = mi_malloc_aligned(size, alignment);
    default:
      break;
  }

  return res;
}

void* MemoryAllocator::ZallocAligned(size_t size, size_t alignment, AllocType type) {
  if (alignment == 0) {
    alignment = alignof(std::max_align_t);
  }

  void* res = nullptr;

  switch (type) {
    case AllocType::STD:
      res = AlignedAlloc(alignment, size);
      memset(res, 0, size);
      break;
    case AllocType::MIMALLOC:
      res = mi_zalloc_aligned(size, alignment);
    default:
      break;
  }

  return res;
}

void* MemoryAllocator::CallocAligned(size_t size, size_t count, size_t alignment, AllocType type) {
  return ZallocAligned(size * count, alignment, type);
}

void* MemoryAllocator::ReallocAligned(void* p, size_t newsize, size_t alignment, AllocType type) {
  if (p) {
    Free(p, type);
  }

  return MallocAligned(newsize, alignment, type);
}

void* MemoryAllocator::MallocAlignedAt(size_t size, size_t alignment, size_t offset, AllocType type) {
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

void* MemoryAllocator::ZallocAlignedAt(size_t size, size_t alignment, size_t offset, AllocType type) {
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

void* MemoryAllocator::CallocAlignedAt(size_t size, size_t count, size_t alignment, size_t offset, AllocType type) {
  return ZallocAlignedAt(size * count, alignment, offset, type);
}

void* MemoryAllocator::ReallocAlignedAt(void* p, size_t newsize, size_t alignment, size_t offset, AllocType type) {
  if (p) {
    Free(p, type);
  }

  return MallocAlignedAt(newsize, alignment, offset, type);
}

void MemoryAllocator::Free(void* p, AllocType type) {
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

}  // namespace RE::Core