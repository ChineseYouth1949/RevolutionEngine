#pragma once

#include "Core.h"

namespace RE {
class RE_API Application {
 public:
  Application();
  virtual ~Application();

  void Run();
};

Application* CreateApplication();
}  // namespace RE