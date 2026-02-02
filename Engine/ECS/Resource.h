#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
struct RE_API Resource {
  Resource() = default;
  ~Resource() { Destroy(); }

  Resource(Resource&& other) noexcept : m_Ptr(other.m_Ptr), m_Deleter(other.m_Deleter), m_Info(std::move(other.m_Info)) {
    other.m_Ptr = nullptr;
    other.m_Deleter = nullptr;
  }

  Resource& operator=(Resource&& other) noexcept {
    if (this != &other) {
      Destroy();
      m_Ptr = other.m_Ptr;
      m_Deleter = other.m_Deleter;
      m_Info = std::move(other.m_Info);
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
    SetInfo<T>();
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

  template <typename T>
  RE_FINLINE const T* Get() const {
    return static_cast<const T*>(m_Ptr);
  }

  RE_FINLINE bool Valid() const { return m_Ptr != nullptr; }
  RE_FINLINE const void* GetPtr() const { return m_Ptr; }
  RE_FINLINE const string& GetInfo() const { return m_Info; }

 private:
  template <typename T>
  void SetInfo() {
    m_Info.clear();
    m_Info += sResouceStr;
    m_Info += typeid(T).name();
    m_Info += sTypeStr;
    m_Info += Convert<string>(stl::to_string(m_Ptr));
  }

  void* m_Ptr{nullptr};
  void (*m_Deleter)(void*) = nullptr;
  string m_Info;

  static inline constexpr std::string_view sResouceStr = "Resouce : type-";
  static inline constexpr std::string_view sTypeStr = " | ptr-";
};

}  // namespace re::engine::ecs