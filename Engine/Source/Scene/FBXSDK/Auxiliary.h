#pragma once

#include "Base/FlagType.h"

#include <string>
#include <vector>

#include <fbxsdk.h>

namespace RE::Core {
class Scene;

void FBXSdkInit();

bool FBXSceneTransform(FbxManager* fbxSdkManager, FbxImporter* fbxImporter, FbxScene* fbxScene, Flag64 falgs, Scene*& resScene,
                       std::vector<std::string>& resErrorInfos);

}  // namespace RE::Core