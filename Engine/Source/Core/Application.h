#pragma once

#include "MacroDefine.h"

namespace RE {
class RE_API Application {
 public:
  Application();
  virtual ~Application();

  void Run();
};

Application* CreateApplication();
}  // namespace RE