#pragma once

#include "Allocator.h"

namespace re::engine::memory {

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::Malloc(size_t size) {
  return mi_malloc(size);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::Zalloc(size_t size) {
  return mi_zalloc(size);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::Calloc(size_t count, size_t size) {
  return mi_calloc(count, size);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::Realloc(void* p, size_t newsize) {
  return mi_realloc(p, newsize);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::ReallocN(void* p, size_t count, size_t newsize) {
  return mi_reallocn(p, count, newsize);
}

template <>
RE_FINLE bool Allocator<AllocType::MiMalloc>::Expand(void* p, size_t size) {
  return mi_expand(p, size);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::MallocAligned(size_t size, size_t alignment) {
  return mi_malloc_aligned(size, alignment);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::ZallocAligned(size_t size, size_t alignment) {
  return mi_zalloc_aligned(size, alignment);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::CallocAligned(size_t count, size_t size, size_t alignment) {
  return mi_calloc_aligned(count, size, alignment);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::ReallocAligned(void* p, size_t newsize, size_t alignment) {
  return mi_realloc_aligned(p, newsize, alignment);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::MallocAlignedAt(size_t size, size_t alignment, size_t offset) {
  return mi_malloc_aligned_at(size, alignment, offset);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::ZallocAlignedAt(size_t size, size_t alignment, size_t offset) {
  return mi_zalloc_aligned_at(size, alignment, offset);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::CallocAlignedAt(size_t count, size_t size, size_t alignment, size_t offset) {
  return mi_calloc_aligned_at(count, size, alignment, offset);
}

template <>
RE_FINLE void* Allocator<AllocType::MiMalloc>::ReallocAlignedAt(void* p, size_t newSize, size_t alignment, size_t offset) {
  return mi_realloc_aligned_at(p, newSize, alignment, offset);
}

template <>
RE_FINLE void Allocator<AllocType::MiMalloc>::Free(void* p) {
  mi_free(p);
}

}  // namespace re::engine::memory