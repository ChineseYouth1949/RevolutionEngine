#pragma once

namespace re::core::memory {

template <typename T>
class StlAllocator<AllocType::STD, T> : public std::allocator<T> {
 public:
  using std::allocator<T>::allocator;

  template <typename U>
  struct rebind {
    using other = StlAllocator<AllocType::STD, U>;
  };
};

template <typename T>
class StlAllocator<AllocType::MiMalloc, T> : public mi_stl_allocator<T> {
 public:
  using mi_stl_allocator<T>::mi_stl_allocator;

  template <typename U>
  struct rebind {
    using other = StlAllocator<AllocType::MiMalloc, U>;
  };
};

}  // namespace re::core::memory