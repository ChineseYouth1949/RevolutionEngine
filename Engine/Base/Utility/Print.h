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
  char buffer[256];
  va_list ap;
  va_start(ap, format);
  vsprintf_s(buffer, 256, format, ap);
  va_end(ap);
  Print(buffer);
}

RE_FINLINE void Printf(const wchar_t* format, ...) {
  wchar_t buffer[256];
  va_list ap;
  va_start(ap, format);
  vswprintf(buffer, 256, format, ap);
  va_end(ap);
  Print(buffer);
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