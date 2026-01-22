#include "Base.h"

inline void CheckRelloc(void* ptr, size_t size) {
  void* newPtr = nullptr;
  newPtr = Allocator<AllocType::MiMalloc>::Realloc(ptr, size);

  if (size != 0) {
    ASSERT_NE(newPtr, nullptr);
    CheckPtrMemoryValid(newPtr, size);
    // Free(newPtr, type);
  } else {
    // ASSERT_EQ(ptr, newPtr);
  }
}

const size_t TestRellocCount = 10;

TEST(TestRelloc, MIMALLOC) {
  const size_t testNum = TestNumBase * TestRellocCount;

  size_t size = 0;
  void* ptr = nullptr;

  CheckRelloc(ptr, 0);

  for (size_t i = 0; i < testNum; i++) {
    size = getRandSize(SMALL_MIN, SMALL_MAX);
    CheckRelloc(ptr, size);
  }
}