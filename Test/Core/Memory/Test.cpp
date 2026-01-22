#include <gtest/gtest.h>

#include "TestMalloc.h"
#include "TestZalloc.h"
#include "TestCalloc.h"
#include "TestRealloc.h"
#include "TestExpand.h"
#include "TestAlign.h"
#include "TestAlignAt.h"
#include "TestAllocator.h"

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}