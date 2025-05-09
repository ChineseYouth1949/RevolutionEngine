#pragma once

#include <iostream>

#include "Engine.h"

extern RE::Engine* RE::CreateEngine();

#ifdef RE_PLATFORM_WINDOWS

int main(int argc, char** argv) {
  std::cout << "RE Engine Start up." << std::endl;
  RE::Engine* app = RE::CreateEngine();
  app->Run();
  delete app;
  return 0;
}

#else
#error only suport windows!
#endif  // RE_PLATFORM_WINDOWS