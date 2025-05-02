#pragma once

#include <iostream>

#include "Core.h"

#ifdef RE_PLATFORM_WINDOWS
extern RE::Application* RE::CreateApplication();

int main(int argc, char** argv) {
  std::cout << "RE Engine Start up." << std::endl;
  RE::Application* app = RE::CreateApplication();
  app->Run();
  delete app;
  return 0;
}

#else
#error only suport windows!
#endif  // RE_PLATFORM_WINDOWS