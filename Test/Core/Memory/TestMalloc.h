#include "Base.h"

inline void CheckMaloc(size_t size) {
  void* ptr = nullptr;
  ptr = Allocator<AllocType::MiMalloc>::Malloc(size);

  if (size != 0) {
    ASSERT_NE(ptr, nullptr);
    CheckPtrMemoryValid(ptr, size);
    Allocator<AllocType::MiMalloc>::Free(ptr);
  } else {
    // ASSERT_EQ(ptr, nullptr);
  }
}

const size_t testMallocCount = 100;

TEST(TestMalloc, MIMALLOC) {
  const size_t testNum = TestNumBase * testMallocCount;

  size_t size = 0;

  CheckMaloc(0);

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(SMALL_MIN, SMALL_MAX);
    CheckMaloc(size);
  }

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(MEDIUM_MIN, MEDIUM_MAX);
    CheckMaloc(size);
  }

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(LARGE_MIN, LARGE_MAX);
    CheckMaloc(size);
  }

  size = NEAR_POWER_1;
  CheckMaloc(size);

  size = NEAR_POWER_2;
  CheckMaloc(size);

  size = UNALIGNED_1;
  CheckMaloc(size);

  size = UNALIGNED_2;
  CheckMaloc(size);

  size = UNALIGNED_3;
  CheckMaloc(size);
}