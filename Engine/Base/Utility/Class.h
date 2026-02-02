#pragma once

#define RE_DECLARE_PRIVATE(Class)    \
 private:                            \
  struct Impl;                       \
  unique_ptr<Impl> m_pImpl;          \
                                     \
 protected:                          \
  RE_FINLINE Impl* d() {             \
    return m_pImpl.get();            \
  }                                  \
  RE_FINLINE const Impl* d() const { \
    return m_pImpl.get();            \
  }

namespace re::engine::utility {
class NonCopyable {
 public:
  NonCopyable() = default;
  ~NonCopyable() = default;

  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;

  NonCopyable(NonCopyable&&) = default;
  NonCopyable& operator=(NonCopyable&&) = default;
};
}  // namespace re::engine::utility