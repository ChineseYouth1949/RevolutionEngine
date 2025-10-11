#pragma once

#include "Base/FlagType.h"

#include <string>

#include <fbxsdk.h>

namespace RE::Core {
class Scene;

void FbxSdkInit();

bool TransformFbxScene(FbxManager* fbxSdkManager, FbxImporter* fbxImporter, FbxScene* fbxScene, Flag64 falgs, Scene*& resScene,
                       std::string& errorInfo);

void FillCameraArray(FbxScene* fbxScene, FbxArray<FbxNode*>& resCameraArray);
}  // namespace RE::Core