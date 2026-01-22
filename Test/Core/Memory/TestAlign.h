#pragma once

#include "Base.h"

inline bool IsAligned(void* ptr, size_t alignment) {
  return (reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0;
}

const size_t testAlignedCount = 1000;

TEST(TestAligned, MIMALLOC) {
  const size_t testNum = TestNumBase * testAlignedCount;
  size_t size = 0;
  void* ptr = nullptr;

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(MEDIUM_MIN, MEDIUM_MAX);
    ptr = Allocator<AllocType::MiMalloc>::MallocAligned(size, 8);
    ASSERT_TRUE(IsAligned(ptr, 8));
  }
}