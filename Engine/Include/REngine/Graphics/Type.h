#pragma once

#include "Base/Base.h"

namespace RE::Engine {

struct RE_DECLSPEC GraphicsCoreConfig {
  HWND wHwnd;
  int wWdith;
  int wHeight;
  std::wstring appRunPath;
  std::wstring resoucePath;
};

}  // namespace RE::Engine