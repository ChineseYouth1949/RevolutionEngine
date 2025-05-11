#pragma once

#include <iostream>

#include "Application.h"

extern RE::Application* RE::CreateApplication();

#ifdef RE_PLATFORM_WINDOWS

int main(int argc, char** argv) {
  std::cout << "RE Application Start up." << std::endl;
  RE::Application* app = RE::CreateApplication();
  app->Run();
  delete app;
  return 0;
}

#else
#error only suport windows!
#endif  // RE_PLATFORM_WINDOWS