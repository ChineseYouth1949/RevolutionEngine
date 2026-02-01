#pragma once

#include "Engine/Base/PCH.h"
#include "Container.h"

#define RE_DEFINE_HANDLE_FACTORY(_Name)                                                                  \
  struct _Name##Handle {                                                                                 \
    size_t id;                                                                                           \
    wstring name;                                                                                        \
    RE_FINLINE bool operator==(const _Name##Handle& other) const { return id == other.id; }              \
  };                                                                                                     \
  class RE_API _Name {                                                                                   \
   public:                                                                                               \
    RE_FINLINE static _Name##Handle CreateHandle(const wstring& name = L"") {                            \
      size_t id = sCount.fetch_add(1);                                                                   \
      return {id, name.empty() ? wstring(L"DynamicID_") + Convert<wstring>(stl::to_wstring(id)) : name}; \
    }                                                                                                    \
                                                                                                         \
    template <typename T>                                                                                \
    RE_FINLINE static const _Name##Handle& GetTypeHandle() {                                             \
      static const _Name##Handle handle = [&]() {                                                        \
        size_t id = sCount.fetch_add(1);                                                                 \
        string n = typeid(T).name();                                                                     \
        return _Name##Handle{id, wstring(n.begin(), n.end())};                                           \
      }();                                                                                               \
      return handle;                                                                                     \
    }                                                                                                    \
                                                                                                         \
   private:                                                                                              \
    static inline std::atomic<size_t> sCount{1};                                                         \
  };

namespace re::engine::utility {
RE_DEFINE_HANDLE_FACTORY(GFactory)
}