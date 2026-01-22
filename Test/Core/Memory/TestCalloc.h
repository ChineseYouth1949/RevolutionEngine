#include "Base.h"

inline void CheckCalloc(size_t size) {
  auto count = getRandSize(0, 10);
  auto ptr = Allocator<AllocType::MiMalloc>::Calloc(count, size);

  if (size * count != 0) {
    ASSERT_NE(ptr, nullptr);
    CheckPtrMemoryValid(ptr, size * count);
    Allocator<AllocType::MiMalloc>::Free(ptr);
  } else {
    // ASSERT_EQ(ptr, nullptr);
  }
}

const size_t testCallocCount = 1;

TEST(TestCalloc, MIMALLOC) {
  const size_t testNum = TestNumBase * testCallocCount;

  size_t size = 0;

  CheckCalloc(0);

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(SMALL_MIN, SMALL_MAX);
    CheckCalloc(size);
  }

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(MEDIUM_MIN, MEDIUM_MAX);
    CheckCalloc(size);
  }

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(LARGE_MIN, LARGE_MAX);
    CheckCalloc(size);
  }

  size = NEAR_POWER_1;
  CheckCalloc(size);

  size = NEAR_POWER_2;
  CheckCalloc(size);

  size = UNALIGNED_1;
  CheckCalloc(size);

  size = UNALIGNED_2;
  CheckCalloc(size);

  size = UNALIGNED_3;
  CheckCalloc(size);
}