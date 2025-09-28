#pragma once

#include "Window.h"

using namespace Engine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
  Window* window = new Window(700, 500, L"First Engine");
  return Win32Applicaiotn::Run(window, hInstance, nCmdShow);
}