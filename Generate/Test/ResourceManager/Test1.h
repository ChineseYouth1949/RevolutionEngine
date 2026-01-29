#pragma once

using namespace re::engine::ecs;

// 模拟复杂资源
struct TestComp {
  size_t val;
  std::vector<int> dummyData;
  static inline std::atomic<int> aliveCount{0};
  TestComp() {
    aliveCount.fetch_add(1);
    dummyData.resize(100, 1);
  }
  ~TestComp() { aliveCount.fetch_sub(1); }
};

class ResourceManagerCorrectness : public ::testing::Test {
 protected:
  ResourceManager rm;
  void SetUp() override { TestComp::aliveCount = 0; }
};

// 1. 测试多线程并发创建/删除的原子性
TEST_F(ResourceManagerCorrectness, ConcurrentCreateRemove) {
  const int threadCount = 12;
  const int opsPerThread = 500;
  std::vector<std::thread> workers;

  for (int i = 0; i < threadCount; ++i) {
    workers.emplace_back([&]() {
      for (int j = 0; j < opsPerThread; ++j) {
        // 多个线程反复对同一个类型 T 进行创建和删除标记
        rm.CreateResource<TestComp>();
        if (j % 2 == 0)
          rm.RemoveResource<TestComp>();
      }
    });
  }

  for (auto& t : workers)
    t.join();

  // 此时 Flush 前，资源不应被物理销毁
  EXPECT_GT(TestComp::aliveCount.load(), 0);

  rm.Flush();  // 执行清理
  // 逻辑：如果最后一次操作是 Remove，Flush 后应该是 0
  // 如果最后一次是 Create，Flush 后应该是 1
  int finalCount = TestComp::aliveCount.load();
  EXPECT_TRUE(finalCount == 0 || finalCount == 1);
}

// 2. 测试“读-删-读”生命周期：验证延迟销毁
TEST_F(ResourceManagerCorrectness, DeferredDeleteSafety) {
  rm.CreateResource<TestComp>();
  auto* initialPtr = rm.GetResource<TestComp>();

  std::atomic<bool> readerRunning{true};
  std::thread reader([&]() {
    while (readerRunning) {
      auto* ptr = rm.GetResource<TestComp>();
      if (ptr) {
        // 模拟读取操作，验证指针即使在其他线程调了 Remove 后依然有效
        EXPECT_EQ(ptr->dummyData.size(), 100);
      }
    }
  });

  std::thread killer([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    rm.RemoveResource<TestComp>();  // 仅仅是标记，内存不应释放
  });

  killer.join();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  readerRunning = false;
  reader.join();

  // 此时内存仍未释放
  EXPECT_EQ(TestComp::aliveCount.load(), 1);
  rm.Flush();
  // 只有 Flush 后内存释放
  EXPECT_EQ(TestComp::aliveCount.load(), 0);
}