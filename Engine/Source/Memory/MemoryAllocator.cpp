#include "Memory/MemoryAllocator.h"

#include <cstdlib>
#include <memory>

#include <mimalloc.h>

namespace RE::Core {

struct MemoryAllocator::Impl {
  AllocateType type;
};

MemoryAllocator::MemoryAllocator(AllocateType type) : m_impl(new Impl()) {
  m_impl->type = type;
}

MemoryAllocator::~MemoryAllocator() {
  delete m_impl;
  m_impl = nullptr;
}

void* MemoryAllocator::Malloc(size_t size) {
  if (size == 0) {
    return nullptr;
  }

  void* res = nullptr;

  switch (m_impl->type) {
    case AllocateType::STD:
      res = std::malloc(size);
      break;
    case AllocateType::Mimalloc:
      res = mi_malloc(size);
    default:
      break;
  }

  return res;
}

void* MemoryAllocator::Zalloc(size_t size) {
  if (size == 0) {
    return nullptr;
  }

  void* res = nullptr;

  switch (m_impl->type) {
    case AllocateType::STD:
      res = std::malloc(size);
      memset(res, 0, size);
      break;
    case AllocateType::Mimalloc:
      res = mi_zalloc(size);
    default:
      break;
  }

  return res;
}

void* MemoryAllocator::Calloc(size_t size, size_t count) {
  return Zalloc(size * count);
}

void* MemoryAllocator::Recalloc(void* p, size_t newSize) {
  if (!p) {
    return Malloc(newSize);
  }

  void* res = nullptr;

  switch (m_impl->type) {
    case AllocateType::STD:
      res = std::realloc(p, newSize);
      break;
    case AllocateType::Mimalloc:
      res = mi_realloc(res, newSize);
    default:
      break;
  }

  return res;
}

void* MemoryAllocator::Recalloc(void* p, size_t newSize, size_t count) {
  return Recalloc(p, newSize * count);
}

bool MemoryAllocator::Expand(void* p, size_t newSize) {
  if (!p) {
    return false;
  }

  void* res = nullptr;

  switch (m_impl->type) {
    case AllocateType::STD:
      break;
    case AllocateType::Mimalloc:
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

void* MemoryAllocator::MallocAligned(size_t size, size_t alignment) {
  if (alignment == 0) {
    alignment = alignof(std::max_align_t);
  }

  void* res = nullptr;

  switch (m_impl->type) {
    case AllocateType::STD:
      res = AlignedAlloc(alignment, size);
      break;
    case AllocateType::Mimalloc:
      res = mi_malloc_aligned(size, alignment);
    default:
      break;
  }

  return res;
}

void* MemoryAllocator::ZallocAligned(size_t size, size_t alignment) {
  if (alignment == 0) {
    alignment = alignof(std::max_align_t);
  }

  void* res = nullptr;

  switch (m_impl->type) {
    case AllocateType::STD:
      res = AlignedAlloc(alignment, size);
      memset(res, 0, size);
      break;
    case AllocateType::Mimalloc:
      res = mi_zalloc_aligned(size, alignment);
    default:
      break;
  }

  return res;
}

void* MemoryAllocator::CallocAligned(size_t size, size_t count, size_t alignment) {
  return ZallocAligned(size * count, alignment);
}

void* MemoryAllocator::ReallocAligned(void* p, size_t newsize, size_t alignment) {
  if (p) {
    Free(p);
  }

  return MallocAligned(newsize, alignment);
}

void* MemoryAllocator::MallocAlignedAt(size_t size, size_t alignment, size_t offset) {
  if (offset >= alignment) {
    return nullptr;
  }

  void* res = nullptr;
  uintptr_t aligned_ptr;

  switch (m_impl->type) {
    case AllocateType::STD:
      res = MallocAligned(size + alignment, alignment);
      aligned_ptr = reinterpret_cast<uintptr_t>(res);
      aligned_ptr = (aligned_ptr + offset + alignment - 1) & ~(alignment - 1);
      res = reinterpret_cast<void*>(aligned_ptr);
      break;
    case AllocateType::Mimalloc:
      res = mi_malloc_aligned_at(size, alignment, offset);
    default:
      break;
  }

  return res;
}

void* MemoryAllocator::ZallocAlignedAt(size_t size, size_t alignment, size_t offset) {
  if (offset >= alignment) {
    return nullptr;
  }

  void* res = nullptr;
  uintptr_t aligned_ptr;

  switch (m_impl->type) {
    case AllocateType::STD:
      res = ZallocAligned(size + alignment, alignment);
      aligned_ptr = reinterpret_cast<uintptr_t>(res);
      aligned_ptr = (aligned_ptr + offset + alignment - 1) & ~(alignment - 1);
      res = reinterpret_cast<void*>(aligned_ptr);
      break;
    case AllocateType::Mimalloc:
      res = mi_malloc_aligned_at(size, alignment, offset);
    default:
      break;
  }

  return res;
}

void* MemoryAllocator::CallocAlignedAt(size_t size, size_t count, size_t alignment, size_t offset) {
  return ZallocAlignedAt(size * count, alignment, offset);
}

void* MemoryAllocator::ReallocAlignedAt(void* p, size_t newsize, size_t alignment, size_t offset) {
  if (p) {
    Free(p);
  }

  return MallocAlignedAt(newsize, alignment, offset);
}

void MemoryAllocator::Free(void* p) {
  if (p) {
    switch (m_impl->type) {
      case AllocateType::STD:
        free(p);
        break;
      case AllocateType::Mimalloc:
        mi_free(p);
      default:
        break;
    }
  }
}

}  // namespace RE::Core