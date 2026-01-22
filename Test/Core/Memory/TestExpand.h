#pragma once

#include "Base.h"

inline void CheckExpand(void* ptr, size_t size) {
  bool success = Allocator<AllocType::MiMalloc>::Expand(ptr, size);

  if (success) {
    CheckPtrMemoryValid(ptr, size);
    // Free(ptr, type);
  }
}

const size_t TestExpandCount = 10;

TEST(TestExpand, MIMALLOC) {
  const size_t testNum = TestNumBase * TestExpandCount;

  size_t size = 0;
  void* ptr = nullptr;

  CheckExpand(ptr, 0);

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(SMALL_MIN, SMALL_MAX);
    CheckExpand(ptr, size);
  }
}