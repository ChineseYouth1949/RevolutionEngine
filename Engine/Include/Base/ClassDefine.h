#pragma once

#include <memory>

#define RE_PIMPL_MEMBER \
  struct Impl;          \
  std::unique_ptr<Impl> m_impl;

#define RE_CLASS_UNCOPYABLE(Class)         \
  Class(const Class&) = delete;            \
  Class(Class&&) = default;                \
  Class& operator=(const Class&) = delete; \
  Class& operator=(Class&&) = default;

#define RE_CLASS_COPYABLE(Class)  \
  Class(const Class&);            \
  Class(Class&&) = default;       \
  Class& operator=(const Class&); \
  Class& operator=(Class&&) = default;