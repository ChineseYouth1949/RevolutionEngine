#pragma once

#include "Engine/Core/Macros.h"

#ifdef RE_USE_EASTL
#define STL_NAMESPACE eastl
RE_FINLE void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line);
RE_FINLE void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags,
                              const char* file, int line);
#include <EASTL/vector.h>
#include <EASTL/queue.h>
#include <EASTL/string.h>
#include <EASTL/map.h>
#include <EASTL/unordered_map.h>
#include <EASTL/list.h>
#include <EASTL/deque.h>
#include <EASTL/set.h>
#include <EASTL/array.h>
#include <EASTL/functional.h>
#include <EASTL/memory.h>
#include <EASTL/algorithm.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/string_view.h>
#include <EASTL/type_traits.h>
#include <EASTL/algorithm.h>
#include <EASTL/span.h>
#include <EASTL/atomic.h>
#else
#define STL_NAMESPACE std
#include <vector>
#include <queue>
#include <string>
#include <map>
#include <unordered_map>
#include <list>
#include <deque>
#include <set>
#include <array>
#include <functional>
#include <memory>
#include <algorithm>
#endif

// use stl::xxx
namespace re::core::stl {
using namespace STL_NAMESPACE;
}  // namespace re::core::stl

#ifdef RE_USE_EASTL
// impl new function
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line) {
  return operator new[](size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file,
                     int line) {
  return operator new[](size);
}
#endif