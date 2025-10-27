#pragma once

#include "Base/CompileDefine.h"

namespace RE::Engine {
class RenderResoucre;

class RE_DECLSPEC RenderCommand {
 public:
  RenderCommand() = default;
  virtual ~RenderCommand() = default;

  virtual bool Execute() = 0;

  virtual bool IsRefResouce() const;
  virtual bool DeRefResouce();
};

}  // namespace RE::Engine