#pragma once

using namespace re::engine::ecs;

// 模拟多类型资源
template <int N>
struct TestComp {
  size_t val = N;
  static inline std::atomic<int> aliveCount{0};
  TestComp() { aliveCount.fetch_add(1); }
  ~TestComp() { aliveCount.fetch_sub(1); }
};

class ResourceManagerTest : public ::testing::Test {
 protected:
  std::unique_ptr<ResourceManager> rm;

  void SetUp() override {
    // 重置计数器
    TestComp<0>::aliveCount = 0;
    TestComp<1>::aliveCount = 0;
    TestComp<99>::aliveCount = 0;
    rm = std::make_unique<ResourceManager>();
  }
};

// 1. 基础创建与获取测试
TEST_F(ResourceManagerTest, BasicCreateGet) {
  auto* p1 = rm->CreateResource<TestComp<0>>();
  ASSERT_NE(p1, nullptr);
  EXPECT_EQ(p1->val, 0);
  EXPECT_EQ(TestComp<0>::aliveCount, 1);

  auto* p2 = rm->GetResource<TestComp<0>>();
  EXPECT_EQ(p1, p2);
}

// 2. 多线程并发创建不同资源 (验证静态数组寻址安全性)
TEST_F(ResourceManagerTest, MultiThreadedCreation) {
  std::vector<std::thread> threads;
  for (int i = 0; i < 4; ++i) {
    threads.emplace_back([this, i]() {
      if (i == 0)
        rm->CreateResource<TestComp<0>>();
      else if (i == 1)
        rm->CreateResource<TestComp<1>>();
      // 更多类型...
    });
  }
  for (auto& t : threads)
    t.join();

  EXPECT_NE(rm->GetResource<TestComp<0>>(), nullptr);
  EXPECT_NE(rm->GetResource<TestComp<1>>(), nullptr);
}

// // 3. 实时销毁安全性测试 (当前设计是调用即销毁)
// TEST_F(ResourceManagerTest, ImmediateRemovalSafety) {
//   using Target = TestComp<99>;
//   rm->CreateResource<Target>();

//   // 线程 A 持续读取
//   std::atomic<bool> stopReader{false};
//   std::thread reader([&]() {
//     while (!stopReader) {
//       auto* ptr = rm->GetResource<Target>();
//       if (ptr) {
//         // 如果能拿到，数据必须有效
//         EXPECT_EQ(ptr->val, 99);
//       }
//       // 如果拿到 nullptr，说明被删了，这也是符合预期的
//     }
//   });

//   std::this_thread::sleep_for(std::chrono::milliseconds(10));

//   // 线程 B 立即销毁
//   rm->RemoveResource<Target>();
//   EXPECT_EQ(Target::aliveCount, 0);  // 应该立即释放内存
//   EXPECT_EQ(rm->GetResource<Target>(), nullptr);

//   stopReader = true;
//   reader.join();
// }

// 4. 测试 Clear 接口
TEST_F(ResourceManagerTest, ClearAll) {
  rm->CreateResource<TestComp<0>>();
  rm->CreateResource<TestComp<1>>();
  EXPECT_EQ(TestComp<0>::aliveCount, 1);
  EXPECT_EQ(TestComp<1>::aliveCount, 1);

  rm->Clear();

  EXPECT_EQ(TestComp<0>::aliveCount, 0);
  EXPECT_EQ(TestComp<1>::aliveCount, 0);
  EXPECT_EQ(rm->GetResource<TestComp<0>>(), nullptr);
}