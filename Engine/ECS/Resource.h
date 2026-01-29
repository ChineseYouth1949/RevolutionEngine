#pragma once

#include "Engine/Base/PCH.h"

namespace re::engine::ecs {
struct RE_API Resource {
  Resource() = default;
  ~Resource() { Destroy(); }

  Resource(const Resource&) = delete;
  Resource& operator=(const Resource&) = delete;

  template <typename T>
  void Create(T* ptr) {
    if (m_Ptr)
      Destroy();

    m_Ptr = static_cast<void*>(ptr);
    m_Deleter = [](void* p) {
      if (p) {
        static_cast<T*>(p)->~T();
        GAlloc::free(p);
      }
    };
  }

  void Destroy() {
    if (m_Ptr && m_Deleter) {
      m_Deleter(m_Ptr);
      m_Ptr = nullptr;
      m_Deleter = nullptr;
    }
  }

  template <typename T>
  RE_FINLINE T* Get() {
    return static_cast<T*>(m_Ptr);
  }

  void* m_Ptr = nullptr;
  void (*m_Deleter)(void*) = nullptr;
};
}  // namespace re::engine::ecs