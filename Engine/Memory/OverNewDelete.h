#pragma once

#include <mimalloc.h>
#include <new>

// --- 1. 标准全局 new 重载 ---
void* operator new(size_t size);
void* operator new[](size_t size);
// C++17 对齐 new
void* operator new(size_t size, std::align_val_t al);
void* operator new[](size_t size, std::align_val_t al);

// --- 3. 标准全局 delete 重载 ---
void operator delete(void* p) noexcept;
void operator delete[](void* p) noexcept;
// C++14 优化版本：带 Size 的释放
void operator delete(void* p, size_t size) noexcept;
void operator delete[](void* p, size_t size) noexcept;
// C++17 对齐释放
void operator delete(void* p, std::align_val_t al) noexcept;
void operator delete[](void* p, std::align_val_t al) noexcept;
void operator delete(void* p, size_t size, std::align_val_t al) noexcept;