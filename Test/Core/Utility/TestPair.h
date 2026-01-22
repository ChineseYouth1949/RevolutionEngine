#include "Base.h"

TEST(TestPair, TestPair) {
  auto infoPair = Pair<int, double>::Make(10, 3.14);
  ASSERT_EQ(infoPair.first, 10);
  ASSERT_EQ(infoPair.second, 3.14);

  PairBS bs = PairBS::Make(false, "Error info");
  if (!bs) {
    stl::string message = bs;
  }
}