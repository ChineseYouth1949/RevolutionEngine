
#pragma once

#include "Base.h"

inline bool IsAlignedAt(void* ptr, size_t alignment, size_t offset) {
  assert((alignment & (alignment - 1)) == 0 && "Alignment must be power of 2");

  uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
  uintptr_t base_address = address - offset;
  return (base_address & (alignment - 1)) == 0;
}

const size_t testAlignAtCount = 1000;

TEST(TestAlignAt, MIMALLOC) {
  const size_t testNum = TestNumBase * testAlignAtCount;
  size_t size = 0;
  void* ptr = nullptr;

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(MEDIUM_MIN, MEDIUM_MAX);
    ptr = Allocator<AllocType::MiMalloc>::MallocAlignedAt(size, 16, 8);
    ASSERT_TRUE(IsAlignedAt(ptr, 16, 8));
  }
}
