#include "Base.h"

inline void CheckZalloc(size_t size) {
  void* ptr = nullptr;
  ptr = Allocator<AllocType::MiMalloc>::Zalloc(size);

  if (size != 0) {
    ASSERT_NE(ptr, nullptr);
    CheckPtrMemoryZero(ptr, size);
    Allocator<AllocType::MiMalloc>::Free(ptr);
  } else {
    // ASSERT_EQ(ptr, nullptr);
  }
}

const size_t testZallocCount = 10;

TEST(TestZalloc, MIMALLOC) {
  const size_t testNum = TestNumBase * testZallocCount;

  size_t size = 0;

  CheckZalloc(0);

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(SMALL_MIN, SMALL_MAX);
    CheckZalloc(size);
  }

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(MEDIUM_MIN, MEDIUM_MAX);
    CheckZalloc(size);
  }

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(LARGE_MIN, LARGE_MAX);
    CheckZalloc(size);
  }

  size = NEAR_POWER_1;
  CheckZalloc(size);

  size = NEAR_POWER_2;
  CheckZalloc(size);

  size = UNALIGNED_1;
  CheckZalloc(size);

  size = UNALIGNED_2;
  CheckZalloc(size);

  size = UNALIGNED_3;
  CheckZalloc(size);
}