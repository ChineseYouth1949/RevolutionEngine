#pragma once

#include <fbxsdk.h>

#include <string>
#include <iostream>

namespace LoadSceneExport {

static const char* gLoadFilename = "./Assets/Model/mech-drone/source/Drone.FBX";
static const char* gSaveFilename = "./Assets/Model/mech-drone/source/Drone-Copy.FBX";

bool LoadScene(FbxManager* pSdkManager, FbxScene* pScene, const char* pFilename);
bool SaveScene(FbxManager* pSdkManager, FbxScene* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia);

int RunExample();

}  // namespace LoadSceneExport