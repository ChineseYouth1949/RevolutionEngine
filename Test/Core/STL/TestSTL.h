#include "Base.h"

TEST(TestSTL, STL) {
  stl::vector<int> a;
  for (size_t i = 0; i < 1000; i++) {
    a.push_back(i);
    ASSERT_EQ(a[i], i);
  }
}