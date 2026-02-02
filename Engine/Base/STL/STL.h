#pragma once

#include "Engine/Base/Macros.h"

#ifdef RE_USE_EASTL
#define STL_NAMESPACE eastl

RE_FINLINE void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line);
RE_FINLINE void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags,
                                const char* file, int line);
RE_FINLINE void* operator new(size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line);
RE_FINLINE void* operator new(size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags,
                              const char* file, int line);

#include <EASTL/vector.h>
#include <EASTL/queue.h>
#include <EASTL/string.h>
#include <EASTL/deque.h>
#include <EASTL/list.h>
#include <EASTL/stack.h>
#include <EASTL/array.h>
#include <EASTL/string.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/fixed_string.h>

#include <EASTL/map.h>
#include <EASTL/set.h>
#include <EASTL/unordered_map.h>
#include <EASTL/unordered_set.h>
#include <EASTL/priority_queue.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/weak_ptr.h>
#include <EASTL/memory.h>
#include <EASTL/initializer_list.h>

#include <EASTL/algorithm.h>
#include <EASTL/functional.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>
#include <EASTL/numeric.h>
#include <EASTL/iterator.h>

#include <EASTL/string_view.h>
#include <EASTL/span.h>
#include <EASTL/optional.h>
#include <EASTL/variant.h>
#include <EASTL/any.h>
#include <EASTL/bit.h>
#include <EASTL/tuple.h>

#include <EASTL/atomic.h>

#else
#define STL_NAMESPACE std

#include <vector>
#include <queue>
#include <string>
#include <deque>
#include <list>
#include <stack>
#include <array>

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <memory>
#include <scoped_allocator>

#include <algorithm>
#include <functional>
#include <type_traits>
#include <utility>
#include <numeric>
#include <iterator>
#include <limits>

#include <string_view>
#include <span>
#include <optional>
#include <variant>
#include <any>
#include <bit>
#include <tuple>
#include <format>

#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#endif

namespace re::engine::stl {
using namespace STL_NAMESPACE;
}

#ifdef RE_USE_EASTL
RE_FINLINE void* operator new(size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line) {
  return ::operator new(size);
}
RE_FINLINE void* operator new(size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags,
                              const char* file, int line) {
  return _aligned_malloc(size, alignment);
}
RE_FINLINE void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line) {
  return ::operator new[](size);
}
RE_FINLINE void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags,
                                const char* file, int line) {
  return _aligned_malloc(size, alignment);
}
#endif

#include <cstdio>
#include <cwchar>
#include <stdarg.h>

RE_FINLINE int Vsnprintf8(char* pDestination, size_t n, const char* pFormat, va_list arguments) {
  return vsnprintf(pDestination, n, pFormat, arguments);
}

RE_FINLINE int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments) {
#ifdef _MSC_VER
  return _vsnwprintf((wchar_t*)pDestination, n, (const wchar_t*)pFormat, arguments);
#else
  return vswprintf((wchar_t*)pDestination, n, (const wchar_t*)pFormat, arguments);
#endif
}

RE_FINLINE int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments) {
// 32位版本通常映射到本地的宽字符处理
// 如果是 Windows 且使用 MSVC，wchar_t 是 16 位的，需根据需求实现
// 简单实现可以调用针对 char32 的处理，或者在 Windows 下暂时强转
#ifdef _MSC_VER
  // Windows 下 wchar_t 只有 16 位，若要支持 32 位可能需要转换，这里提供占位实现
  return -1;
#else
  return vswprintf((wchar_t*)pDestination, n, (const wchar_t*)pFormat, arguments);
#endif
}

#if defined(EA_CHAR8_UNIQUE) && EA_CHAR8_UNIQUE
RE_FINLINE int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments) {
  return vsnprintf((char*)pDestination, n, (const char*)pFormat, arguments);
}
#endif

#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
RE_FINLINE int VsnprintfW(wchar_t* pDestination, size_t n, const wchar_t* pFormat, va_list arguments) {
#ifdef _MSC_VER
  return _vsnwprintf(pDestination, n, pFormat, arguments);
#else
  return vswprintf(pDestination, n, pFormat, arguments);
#endif
}
#endif