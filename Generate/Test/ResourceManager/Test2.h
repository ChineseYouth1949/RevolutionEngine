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

#include <random>
#include <numeric>

// 模拟资源
struct BenchResource {
  float data[16];  // 64 bytes, 刚好一个 Cache Line
};

// 预准备随机索引，排除随机数生成对性能的影响
static vector<size_t> GenerateRandomIndices(size_t size, size_t count) {
  vector<size_t> indices(count);
  std::mt19937 gen(42);
  std::uniform_int_distribution<size_t> dist(0, size - 1);
  for (size_t i = 0; i < count; ++i)
    indices[i] = dist(gen);
  return indices;
}

// 方案 1: 你的 ResourceManager 方案
static void BM_ResourceManager_RandomGet(benchmark::State& state) {
  ResourceManager rm;
  const size_t num_elements = ResIdFactory::sMaxId;
  // 填充资源
  for (size_t i = 0; i < 100; ++i)
    rm.CreateResource<BenchResource>();

  auto indices = GenerateRandomIndices(100, 10000);
  size_t i = 0;

  for (auto _ : state) {
    // 模拟随机获取
    auto* res = rm.GetResource<BenchResource>();  // 实际逻辑会根据类型获取固定ID
    benchmark::DoNotOptimize(res);
    if (++i >= indices.size())
      i = 0;
  }
}
BENCHMARK(BM_ResourceManager_RandomGet);

// 方案 2: 原生 vector 方案 (预分配)
static void BM_StdVector_RandomGet(benchmark::State& state) {
  struct ResourceStub {
    void* ptr;
  };
  const size_t num_elements = ResIdFactory::sMaxId;
  vector<ResourceStub> v(num_elements);

  // 模拟填充
  BenchResource* mock_ptr = new BenchResource();
  for (size_t i = 0; i < 100; ++i)
    v[i].ptr = mock_ptr;

  auto indices = GenerateRandomIndices(100, 10000);
  size_t i = 0;

  for (auto _ : state) {
    // 直接下标访问
    auto* res = v[indices[i]].ptr;
    benchmark::DoNotOptimize(res);
    if (++i >= indices.size())
      i = 0;
  }
  delete mock_ptr;
}
BENCHMARK(BM_StdVector_RandomGet);