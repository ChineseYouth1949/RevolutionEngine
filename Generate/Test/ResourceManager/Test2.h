#pragma once

#include <shared_mutex>

// 方案 A: 标准容器 + 读写锁
struct MutexVectorManager {
  vector<void*> data;
  std::shared_mutex mtx;
  MutexVectorManager() { data.resize(1000, nullptr); }
  void* Get(size_t id) {
    std::shared_lock lock(mtx);
    return data[id];
  }
};

// 方案 B: Map + Any (典型的脚本系统实现)
struct AnyMapManager {
  std::unordered_map<std::type_index, std::any> data;
  std::mutex mtx;

  template <typename T>
  T* Get() {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = data.find(typeid(T));
    if (it != data.end()) {
      // 正确用法：直接取出存储在 any 里的指针值
      return std::any_cast<T*>(it->second);
    }
    return nullptr;
  }
};
// --- 测试你的 ResourceManager ---
static void BM_YourAtomicManager_Get(benchmark::State& state) {
  static ResourceManager rm;
  rm.CreateResource<TestComp>();
  for (auto _ : state) {
    benchmark::DoNotOptimize(rm.GetResource<TestComp>());
  }
}
BENCHMARK(BM_YourAtomicManager_Get)->Threads(1)->Threads(8);

// --- 测试 Mutex + Vector ---
static void BM_MutexVector_Get(benchmark::State& state) {
  static MutexVectorManager mvm;
  for (auto _ : state) {
    benchmark::DoNotOptimize(mvm.Get(10));
  }
}
BENCHMARK(BM_MutexVector_Get)->Threads(1)->Threads(8);

// --- 测试 Map + Any ---
static void BM_AnyMap_Get(benchmark::State& state) {
  static AnyMapManager amm;
  for (auto _ : state) {
    benchmark::DoNotOptimize(amm.Get<TestComp>());
  }
}
BENCHMARK(BM_AnyMap_Get)->Threads(1)->Threads(8);