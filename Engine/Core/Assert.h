#pragma once

#include <assert.h>
#include <tchar.h>

#ifdef RE_DEBUG
#define RE_ASSERT(expr)                                                                                                                         \
  do {                                                                                                                                          \
    bool result = (expr);                                                                                                                       \
    if (!result) {                                                                                                                              \
      TCHAR msg[1024];                                                                                                                          \
      _stprintf_s(msg, _T("Assertion failed!\n\nFile: %s\nLine: %d\nFunction: %s\n\nExpression: %s\nEvaluated to: %s"), _T(__FILE__), __LINE__, \
                  _T(__FUNCTION__), _T(#expr), result ? _T("true") : _T("false"));                                                              \
      MessageBox(NULL, msg, _T("REAssert"), MB_ICONERROR | MB_OK);                                                                              \
      DebugBreak();                                                                                                                             \
    }                                                                                                                                           \
  } while (0)

#define RE_ASSERT_MSG(expr, message)                                                                                                       \
  do {                                                                                                                                     \
    bool result = (expr);                                                                                                                  \
    if (!result) {                                                                                                                         \
      TCHAR msg[1024];                                                                                                                     \
      _stprintf_s(msg, _T("Assertion failed!\n\nFile: %s\nLine: %d\nFunction: %s\n\nExpression: %s\nMessage: %s"), _T(__FILE__), __LINE__, \
                  _T(__FUNCTION__), _T(#expr), _T(message));                                                                               \
      MessageBox(NULL, msg, _T("REAssertMsg"), MB_ICONERROR | MB_OK);                                                                      \
      DebugBreak();                                                                                                                        \
    }                                                                                                                                      \
  } while (0)

#define RE_ASSERT_HR(expr)                                                                                                                  \
  do {                                                                                                                                      \
    HRESULT hr_internal = (expr);                                                                                                           \
    if (FAILED(hr_internal)) {                                                                                                              \
      TCHAR msg[1024];                                                                                                                      \
      /* 使用 %hs 明确告诉编译器这些参数是窄字符串 (const char*) */                                                                         \
      _stprintf_s(msg, _T("HRESULT Assertion Failed!\n\nFile: %hs\nLine: %d\nFunction: %hs\n\nExpression: %hs\nHRESULT: 0x%08X"), __FILE__, \
                  __LINE__, __FUNCTION__, #expr, (unsigned int)hr_internal);                                                                \
      MessageBox(NULL, msg, _T("REAssertHR"), MB_ICONERROR | MB_OK);                                                                        \
      DebugBreak();                                                                                                                         \
    }                                                                                                                                       \
  } while (0)

#else

#define RE_ASSERT(expr) (void)(expr)
#define RE_ASSERT_MSG(expr, message) (void)(expr)
#define RE_ASSERT_HR(expr) (void)(expr)

#endif

#define RE_STATIC_ASSERT(condition, message) static_assert(condition, message)