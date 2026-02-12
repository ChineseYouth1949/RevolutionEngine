#include "OverNewDelete.h"
#include "Allocator.h"

constexpr re::engine::memory::AllocType OverNDAllocType = re::engine::memory::AllocType::MiMalloc;
using OverNDAlloc = re::engine::memory::Allocator<OverNDAllocType>;

// --- 1. 标准全局 new 重载 ---
void* operator new(size_t size) {
  return OverNDAlloc::malloc(size);
}
void* operator new[](size_t size) {
  return OverNDAlloc::malloc(size);
}
// C++17 对齐 new
void* operator new(size_t size, std::align_val_t al) {
  return OverNDAlloc::mallocAligned(size, (size_t)al);
}
void* operator new[](size_t size, std::align_val_t al) {
  return OverNDAlloc::mallocAligned(size, (size_t)al);
}

// --- 3. 标准全局 delete 重载 ---
void operator delete(void* p) noexcept {
  OverNDAlloc::free(p);
}
void operator delete[](void* p) noexcept {
  OverNDAlloc::free(p);
}
// C++14 优化版本：带 Size 的释放
void operator delete(void* p, size_t size) noexcept {
  OverNDAlloc::free(p);
}
void operator delete[](void* p, size_t size) noexcept {
  OverNDAlloc::free(p);
}
// C++17 对齐释放
void operator delete(void* p, std::align_val_t al) noexcept {
  OverNDAlloc::free(p);
}
void operator delete[](void* p, std::align_val_t al) noexcept {
  OverNDAlloc::free(p);
}
void operator delete(void* p, size_t size, std::align_val_t al) noexcept {
  OverNDAlloc::free(p);
}