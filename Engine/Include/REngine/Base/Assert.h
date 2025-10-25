#pragma once

#include <assert.h>
#include <tchar.h>

#include "WindowsHeader.h"

#ifdef RE_DEBUG

#define REAssert(value)                                                                                                            \
  do {                                                                                                                             \
    if (!(value)) {                                                                                                                \
      TCHAR msg[1024];                                                                                                             \
      _stprintf_s(msg, _T("Assertion failed!\n\nFile: %s\nLine: %d\nFunction: %s\n\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__)); \
      MessageBox(NULL, msg, _T("Assertion Failed"), MB_ICONERROR | MB_OK);                                                         \
      DebugBreak();                                                                                                                \
    }                                                                                                                              \
  } while (0);

#define REAssertInfo(value, info)                                                                                                            \
  do {                                                                                                                                       \
    if (!(value)) {                                                                                                                          \
      TCHAR msg[1024];                                                                                                                       \
      _stprintf_s(msg, _T("Assertion failed!\n\nFile: %s\nLine: %d\nFunction: %s\n\nMessage: %s"), _T(__FILE__), __LINE__, _T(__FUNCTION__), \
                  _T(info));                                                                                                                 \
      MessageBox(NULL, msg, _T("Assertion Failed"), MB_ICONERROR | MB_OK);                                                                   \
      DebugBreak();                                                                                                                          \
    }                                                                                                                                        \
  } while (0);

#else

#define REAssert(value)
#define REAssertInfo(value, info)

#endif