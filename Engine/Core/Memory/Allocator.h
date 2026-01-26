#pragma once

#include "Engine/Core/Base.h"

namespace re::engine::memory {

enum struct AllocType : std::uint8_t { STD, MiMalloc };

// Support STL and MiMalloc
template <AllocType type, typename T>
class StlAllocator;

/*
*  Allocator not supported AllocType::STD, because MSVC cannot implement std::align_malloc, 
*  and Windows memory allocation requires the separate use of free and _aligned_free to release memory, 
*  which does not conform to the unified free interface. 
*  In short, using mimalloc to implement the Allocator performs well enough.
*/
template <AllocType type>
class Allocator {
 public:
  // No type alloc
  RE_FINLE static void* Malloc(size_t size);
  RE_FINLE static void* Zalloc(size_t size);
  RE_FINLE static void* Calloc(size_t count, size_t size);

  RE_FINLE static void* Realloc(void* p, size_t newSize);
  RE_FINLE static void* ReallocN(void* p, size_t count, size_t newSize);
  RE_FINLE static bool Expand(void* p, size_t newSize);

  RE_FINLE static void* MallocAligned(size_t size, size_t alignment);
  RE_FINLE static void* ZallocAligned(size_t size, size_t alignment);
  RE_FINLE static void* CallocAligned(size_t count, size_t size, size_t alignment);
  RE_FINLE static void* ReallocAligned(void* p, size_t newsize, size_t alignment);

  RE_FINLE static void* MallocAlignedAt(size_t size, size_t alignment, size_t offset);
  RE_FINLE static void* ZallocAlignedAt(size_t size, size_t alignment, size_t offset);
  RE_FINLE static void* CallocAlignedAt(size_t count, size_t size, size_t alignment, size_t offset);
  RE_FINLE static void* ReallocAlignedAt(void* p, size_t newsize, size_t alignment, size_t offset);

  RE_FINLE static void Free(void* p);

  // Type alloc
  template <typename T>
  RE_FINLE static T* Alloc() {
    void* p = nullptr;
    p = Malloc(sizeof(T));
    return static_cast<T*>(p);
  }

  template <typename T>
  RE_FINLE static T* AllocZero() {
    void* p = nullptr;
    p = Zalloc(sizeof(T));
    return static_cast<T*>(p);
  }

  template <typename T, typename... Args>
  RE_FINLE static T* Create(Args&&... args) {
    void* p = Malloc(sizeof(T));
    return new (p) T(std::forward<Args>(args)...);
  }

  template <typename T>
  RE_FINLE static void Destroy(T* ptr) {
    if (!ptr) {
      return;
    }

    ptr->~T();
    void* p = static_cast<void*>(ptr);
    Free(p);
  }

  template <typename T>
  struct Deleter {
    void operator()(T* p) const noexcept { Destroy<T>(p); }
  };

  template <typename T>
  using SharedPtr = stl::shared_ptr<T>;

  template <typename T, typename... Args>
  RE_FINLE static SharedPtr<T> CreateSharedPtr(Args&&... args) {
    T* ptr = Create<T>(std::forward<Args>(args)...);
    return SharedPtr<T>(ptr, Deleter<T>());
  }

  template <typename T>
  using UniquePtr = stl::unique_ptr<T, Deleter<T>>;

  template <typename T, typename... Args>
  RE_FINLE static UniquePtr<T> CreateUniquePtr(Args&&... args) {
    T* ptr = Create<T>(std::forward<Args>(args)...);
    return UniquePtr<T>(ptr);
  }

  template <typename T>
  using vector = stl::vector<T, StlAllocator<type, T>>;

  template <typename T>
  using deque = stl::deque<T, StlAllocator<type, T>>;

  template <typename T>
  using list = stl::list<T, StlAllocator<type, T>>;

  template <typename T, size_t N>
  using array = stl::array<T, N>;

  template <typename T>
  using set = stl::set<T, stl::less<T>, StlAllocator<type, T>>;

  template <typename Key, typename Value>
  using map = stl::map<Key, Value, stl::less<Key>, StlAllocator<type, stl::pair<const Key, Value>>>;

  template <typename T>
  using multiset = stl::multiset<T, stl::less<T>, StlAllocator<type, T>>;

  template <typename T>
  using hash_set = stl::unordered_set<T, stl::hash<T>, stl::equal_to<T>, StlAllocator<type, T>>;

  template <typename Key, typename Value>
  using hash_map = stl::unordered_map<Key, Value, stl::hash<Key>, stl::equal_to<Key>, StlAllocator<type, stl::pair<const Key, Value>>>;

  template <typename T>
  using queue = stl::queue<T, deque<T>>;

  template <typename T>
  using stack = stl::stack<T, deque<T>>;

  template <typename T, typename Container = vector<T>, typename Compare = stl::less<typename Container::value_type>>
  using priority_queue = stl::priority_queue<T, Container, Compare>;

  template <typename T>
  using basic_string = stl::basic_string<T, StlAllocator<type, T>>;

  using string = basic_string<char>;
  using wstring = basic_string<wchar_t>;
};

}  // namespace re::engine::memory

namespace re::engine {
// Global Memory Allocator
const memory::AllocType GlobalAllocType = memory::AllocType::MiMalloc;
using Alloc = memory::Allocator<GlobalAllocType>;
}  // namespace re::engine

// Convenient macro
#define RE_CLASS_ALLOCATOR(Type) using Alloc = re::engine::memory::Allocator<re::engine::memory::AllocType::Type>;
#define RE_NAME_ALLOCATOR(Name, Type) using Name = re::engine::memory::Allocator<re::engine::memory::AllocType::Type>;

#include "StlAllocator.h"
#include "Mimalloc.h"