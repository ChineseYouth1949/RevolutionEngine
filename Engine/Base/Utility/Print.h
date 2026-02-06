#pragma once

#include "Engine/Base/PCH.h"

namespace re::engine::utility {
#ifdef _CONSOLE
RE_FINLINE void Print(const char* msg) {
  printf("%s", msg);
}
RE_FINLINE void Print(const wchar_t* msg) {
  wprintf(L"%ws", msg);
}
#else
RE_FINLINE void Print(const char* msg) {
  OutputDebugStringA(msg);
}
RE_FINLINE void Print(const wchar_t* msg) {
  OutputDebugString(msg);
}
#endif

RE_FINLINE void Printf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  va_list ap_copy;
  va_copy(ap_copy, ap);
#if defined(_MSC_VER)
  int size = _vscprintf(format, ap_copy);
#else
  int size = std::vsnprintf(nullptr, 0, format, ap_copy);
#endif
  va_end(ap_copy);
  if (size < 0) size = 0;
  std::string buf(static_cast<size_t>(size) + 1, '\0');
#if defined(_MSC_VER)
  vsprintf_s(buf.data(), buf.size(), format, ap);
#else
  std::vsnprintf(buf.data(), buf.size(), format, ap);
#endif
  va_end(ap);
  if (!buf.empty() && buf.back() == '\0') buf.pop_back();
  Print(buf.c_str());
}

RE_FINLINE void Printf(const wchar_t* format, ...) {
  va_list ap;
  va_start(ap, format);
  va_list ap_copy;
  va_copy(ap_copy, ap);
#if defined(_MSC_VER)
  int size = _vscwprintf(format, ap_copy);
#else
  int size = std::vswprintf(nullptr, 0, format, ap_copy);
#endif
  va_end(ap_copy);
  if (size < 0) size = 0;
  std::wstring buf(static_cast<size_t>(size) + 1, L'\0');
#if defined(_MSC_VER)
  vswprintf_s(buf.data(), buf.size(), format, ap);
#else
  std::vswprintf(buf.data(), buf.size(), format, ap);
#endif
  va_end(ap);
  if (!buf.empty() && buf.back() == L'\0') buf.pop_back();
  Print(buf.c_str());
}

RE_FINLINE void PrintSubMessage(const char* format, ...) {
  Print("--> ");
  char buffer[256];
  va_list ap;
  va_start(ap, format);
  vsprintf_s(buffer, 256, format, ap);
  va_end(ap);
  Print(buffer);
  Print("\n");
}
RE_FINLINE void PrintSubMessage(const wchar_t* format, ...) {
  Print("--> ");
  wchar_t buffer[256];
  va_list ap;
  va_start(ap, format);
  vswprintf(buffer, 256, format, ap);
  va_end(ap);
  Print(buffer);
  Print("\n");
}
RE_FINLINE void PrintSubMessage(void) {}

}  // namespace re::engine::utility