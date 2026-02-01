#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
struct RE_API Resource {
  Resource() = default;
  ~Resource() { Destroy(); }

  Resource(Resource&& other) noexcept : m_Ptr(other.m_Ptr), m_Deleter(other.m_Deleter) {
    other.m_Ptr = nullptr;
    other.m_Deleter = nullptr;
  }

  Resource& operator=(Resource&& other) noexcept {
    if (this != &other) {
      Destroy();
      m_Ptr = other.m_Ptr;
      m_Deleter = other.m_Deleter;
      other.m_Ptr = nullptr;
      other.m_Deleter = nullptr;
    }
    return *this;
  }

  template <typename T>
  void Create(T* ptr) {
    Destroy();

    m_Deleter = [](void* p) {
      if (p) {
        GAlloc::destroy<T>(static_cast<T*>(p));
      }
    };

    m_Ptr = ptr;
  }

  template <typename T>
  RE_FINLINE T* Get() {
    return static_cast<T*>(m_Ptr);
  }

  void Destroy() {
    if (m_Ptr && m_Deleter) {
      m_Deleter(m_Ptr);
      m_Ptr = nullptr;
      m_Deleter = nullptr;
    }
  }

 private:
  void* m_Ptr{nullptr};
  void (*m_Deleter)(void*) = nullptr;
};

}  // namespace re::engine::ecs