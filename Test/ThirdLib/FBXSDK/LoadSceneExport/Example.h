#pragma once

#include <windows.h>

namespace LoadSceneExport {

static const char* gLoadFilename = "./Assets/Model/mech-drone/source/Drone.FBX";
static const char* gSaveFilename = "./Assets/Model/mech-drone/source/Drone-Copy.FBX";

int RunExample(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

}  // namespace LoadSceneExport