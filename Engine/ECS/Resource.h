#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
struct RE_API Resource {
  Resource() = default;
  ~Resource() { Destroy(); }

  Resource(const Resource&) = delete;
  Resource& operator=(const Resource&) = delete;

  Resource(Resource&& other) noexcept
      : m_Ptr(other.m_Ptr.exchange(nullptr, std::memory_order_acq_rel)), m_Deleter(std::exchange(other.m_Deleter, nullptr)) {}
  Resource& operator=(Resource&& other) noexcept {
    if (this != &other) {
      Destroy();
      m_Ptr.store(other.m_Ptr.exchange(nullptr, std::memory_order_acq_rel), std::memory_order_release);
      m_Deleter = std::exchange(other.m_Deleter, nullptr);
    }
    return *this;
  }

  template <typename T>
  void Create(T* ptr) {
    std::lock_guard<std::mutex> lock(m_SlotLock);
    DestroyNoLock();

    m_Deleter = [](void* p) {
      if (p) {
        static_cast<T*>(p)->~T();
        GAlloc::free(p);
      }
    };
    m_Ptr.store(static_cast<void*>(ptr), std::memory_order_release);
  }

  template <typename T>
  RE_FINLINE T* Get() {
    return static_cast<T*>(m_Ptr.load(std::memory_order_acquire));
  }

  void Destroy() {
    std::lock_guard<std::mutex> lock(m_SlotLock);
    DestroyNoLock();
  }

  void DestroyNoLock() {
    void* p = m_Ptr.load(std::memory_order_relaxed);
    if (p && m_Deleter) {
      m_Deleter(p);
      m_Ptr.store(nullptr, std::memory_order_release);
      m_Deleter = nullptr;
    }
  }

 private:
  std::atomic<void*> m_Ptr{nullptr};
  void (*m_Deleter)(void*) = nullptr;
  std::mutex m_SlotLock;
};

}  // namespace re::engine::ecs