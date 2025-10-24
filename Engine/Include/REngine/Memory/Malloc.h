#pragma once

#include <cinttypes>

#include "Base/CompileDefine.h"

namespace RE::Engine {

enum struct AllocType : uint8_t { STD = 0, MIMALLOC };

RE_DECLSPEC void* Malloc(size_t size, AllocType type);
RE_DECLSPEC void* Zalloc(size_t size, AllocType type);
RE_DECLSPEC void* Calloc(size_t size, size_t count, AllocType type);

RE_DECLSPEC void* Realloc(void* p, size_t newSize, AllocType type);
RE_DECLSPEC void* ReallocN(void* p, size_t newSize, size_t count, AllocType type);
RE_DECLSPEC bool Expand(void* p, size_t newSize, AllocType type);

RE_DECLSPEC void* MallocAligned(size_t size, size_t alignment, AllocType type);
RE_DECLSPEC void* ZallocAligned(size_t size, size_t alignment, AllocType type);
RE_DECLSPEC void* CallocAligned(size_t size, size_t count, size_t alignment, AllocType type);
RE_DECLSPEC void* ReallocAligned(void* p, size_t newsize, size_t alignment, AllocType type);

RE_DECLSPEC void* MallocAlignedAt(size_t size, size_t alignment, size_t offset, AllocType type);
RE_DECLSPEC void* ZallocAlignedAt(size_t size, size_t alignment, size_t offset, AllocType type);
RE_DECLSPEC void* CallocAlignedAt(size_t size, size_t count, size_t alignment, size_t offset, AllocType type);
RE_DECLSPEC void* ReallocAlignedAt(void* p, size_t newsize, size_t alignment, size_t offset, AllocType type);

RE_DECLSPEC void Free(void* p, AllocType type);

}  // namespace RE::Engine