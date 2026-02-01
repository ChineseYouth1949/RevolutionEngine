#pragma once

#include <shared_mutex>

using namespace re::engine::ecs;

// --- 对照组 B: Map + Any ---
struct AnyMapManager {
  std::unordered_map<std::type_index, std::any> data;
  std::mutex mtx;

  template <typename T>
  T* Get() {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = data.find(typeid(T));
    return (it != data.end()) ? std::any_cast<T*>(it->second) : nullptr;
  }

  template <typename T>
  void Put(T* ptr) {
    std::lock_guard<std::mutex> lock(mtx);
    data[typeid(T)] = ptr;
  }
};

// --- 基准测试：热点读取性能 ---

static void BM_YourAtomicManager_Get(benchmark::State& state) {
  auto rm = std::make_unique<ResourceManager>();
  rm->CreateResource<TestComp<1>>();

  for (auto _ : state) {
    // 这一步包含了：IdFactory查表 + 数组索引 + Atomic Load
    benchmark::DoNotOptimize(rm->GetResource<TestComp<1>>());
  }
}
// 测试单线程与 8 线程并发竞争下的表现
BENCHMARK(BM_YourAtomicManager_Get)->Threads(1)->Threads(8);

static void BM_AnyMap_Get(benchmark::State& state) {
  AnyMapManager amm;
  amm.Put<TestComp<1>>(new TestComp<1>());

  for (auto _ : state) {
    // 这一步包含了：Mutex Lock + Hash Map Find
    benchmark::DoNotOptimize(amm.Get<TestComp<1>>());
  }
}
BENCHMARK(BM_AnyMap_Get)->Threads(1)->Threads(8);

// --- 基准测试：内存分布与伪共享效应 ---

static void BM_ResourceManager_RandomAccess(benchmark::State& state) {
  auto rm = std::make_unique<ResourceManager>();
  // 模拟填充一些资源
  rm->CreateResource<TestComp<1>>();
  rm->CreateResource<TestComp<10>>();
  rm->CreateResource<TestComp<50>>();

  for (auto _ : state) {
    // 在多个不同 ID 之间跳转访问
    benchmark::DoNotOptimize(rm->GetResource<TestComp<1>>());
    benchmark::DoNotOptimize(rm->GetResource<TestComp<10>>());
    benchmark::DoNotOptimize(rm->GetResource<TestComp<50>>());
  }
}
BENCHMARK(BM_ResourceManager_RandomAccess)->Threads(4);