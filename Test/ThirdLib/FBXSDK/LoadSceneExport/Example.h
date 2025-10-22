#pragma once

#include <windows.h>

namespace LoadSceneExport {

static const char* gLoadFilename = "./Resource/Model/mech-drone/source/Drone.FBX";
static const char* gSaveFilename = "./Resource/Model/mech-drone/source/Drone-Copy.FBX";

int RunExample(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

}  // namespace LoadSceneExport