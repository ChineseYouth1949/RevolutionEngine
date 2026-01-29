#pragma once

#include "Print.h"

#ifdef ERROR
#undef ERROR
#endif
#ifdef ASSERT
#undef ASSERT
#endif
#ifdef HALT
#undef HALT
#endif

#define RE_HALT(...) ERROR(__VA_ARGS__) __debugbreak();

#ifdef RELEASE

#define RE_ASSERT(isTrue, ...) (void)(isTrue)
#define RE_WARN_ONCE_IF(isTrue, ...) (void)(isTrue)
#define RE_WARN_ONCE_IF_NOT(isTrue, ...) (void)(isTrue)
#define RE_ERROR(msg, ...)
#define RE_DEBUGPRINT(msg, ...) \
  do {                          \
  } while (0)
#define RE_ASSERT_SUCCEEDED(hr, ...) (void)(hr)

#else  // !RELEASE

#define RE_STRINGIFY(x) #x
#define RE_STRINGIFY_BUILTIN(x) RE_STRINGIFY(x)
#define RE_ASSERT(isFalse, ...)                                                                                                    \
  if (!(bool)(isFalse)) {                                                                                                          \
    re::engine::utility::Print("\nAssertion failed in " RE_STRINGIFY_BUILTIN(__FILE__) " @ " RE_STRINGIFY_BUILTIN(__LINE__) "\n"); \
    re::engine::utility::PrintSubMessage("\'" #isFalse "\' is false");                                                             \
    re::engine::utility::PrintSubMessage(__VA_ARGS__);                                                                             \
    re::engine::utility::Print("\n");                                                                                              \
    __debugbreak();                                                                                                                \
  }

#define RE_ASSERT_SUCCEEDED(hr, ...)                                                                                             \
  if (FAILED(hr)) {                                                                                                              \
    re::engine::utility::Print("\nHRESULT failed in " RE_STRINGIFY_BUILTIN(__FILE__) " @ " RE_STRINGIFY_BUILTIN(__LINE__) "\n"); \
    re::engine::utility::PrintSubMessage("hr = 0x%08X", hr);                                                                     \
    re::engine::utility::PrintSubMessage(__VA_ARGS__);                                                                           \
    re::engine::utility::Print("\n");                                                                                            \
    __debugbreak();                                                                                                              \
  }

#define RE_WARN_ONCE_IF(isTrue, ...)                                                                                               \
  {                                                                                                                                \
    static bool s_TriggeredWarning = false;                                                                                        \
    if ((bool)(isTrue) && !s_TriggeredWarning) {                                                                                   \
      s_TriggeredWarning = true;                                                                                                   \
      re::engine::utility::Print("\nWarning issued in " RE_STRINGIFY_BUILTIN(__FILE__) " @ " RE_STRINGIFY_BUILTIN(__LINE__) "\n"); \
      re::engine::utility::PrintSubMessage("\'" #isTrue "\' is true");                                                             \
      re::engine::utility::PrintSubMessage(__VA_ARGS__);                                                                           \
      re::engine::utility::Print("\n");                                                                                            \
    }                                                                                                                              \
  }

#define RE_WARN_ONCE_IF_NOT(isTrue, ...) RE_WARN_ONCE_IF(!(isTrue), __VA_ARGS__)

#define RE_ERROR(...)                                                                                                          \
  re::engine::utility::Print("\nError reported in " RE_STRINGIFY_BUILTIN(__FILE__) " @ " RE_STRINGIFY_BUILTIN(__LINE__) "\n"); \
  re::engine::utility::PrintSubMessage(__VA_ARGS__);                                                                           \
  re::engine::utility::Print("\n");

#define RE_DEBUGPRINT(msg, ...) re::engine::utility::Printf(msg "\n", ##__VA_ARGS__);

#endif

#define RE_BreakIfFailed(hr) \
  if (FAILED(hr))            \
  __debugbreak()