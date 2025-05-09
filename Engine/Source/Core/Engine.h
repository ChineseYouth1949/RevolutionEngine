#pragma once

#include "MacroDefine.h"

namespace RE {
class RE_API Engine {
 public:
  Engine();
  virtual ~Engine();

  void Run();
};

Engine* CreateEngine();
}  // namespace RE