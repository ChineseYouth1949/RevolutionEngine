#pragma once

#include "Base/CompileDefine.h"

#include <cinttypes>
#include <atomic>

namespace RE::Engine {

class RE_DECLSPEC RenderResource {
 public:
  using Handle = uint64_t;

  enum struct Type {
    None = 0,
    Pipeline,
  };

  RenderResource(Type type) : mType(type), mHandle(sHandle++) {}
  virtual ~RenderResource() = default;

  virtual bool IsLoad() const = 0;
  virtual bool Load() = 0;
  virtual bool Release() = 0;

  RE_INLINE Type GetType() const { return mType; }
  RE_INLINE Handle GetHandle() const { return mHandle; }

 private:
  const Type mType;
  const Handle mHandle;

  static inline std::atomic<Handle> sHandle = 0;
};

}  // namespace RE::Engine