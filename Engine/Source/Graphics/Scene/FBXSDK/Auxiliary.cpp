#include <mutex>
#include <filesystem>

#include "Auxiliary.h"

#include "Base/FlagType.h"

#include "Memory/Memory.h"

#include "Graphics/Scene/Scene.h"
#include "Graphics/Scene/ISceneImporter.h"

namespace RE::Core {

void FbxSdkInit() {
  static std::mutex sMutex;
  static std::atomic<bool> sInited = false;

  if (!sInited) {
    std::lock_guard lock(sMutex);

    FbxSetMallocHandler(GMalloc);
    FbxSetCallocHandler(GCalloc);
    FbxSetReallocHandler(GRealloc);
    FbxSetFreeHandler(GFree);

    sInited = true;
  }
}

bool FileExit(std::string filename) {
  return std::filesystem::exists(filename);
}

void LoadCacheRecursive(FbxScene* pScene, FbxAnimLayer* pAnimLayer, const char* pFbxFileName) {
  const int textureCount = pScene->GetTextureCount();
  for (size_t i = 0; i < textureCount; i++) {
    FbxTexture* texture = pScene->GetTexture(i);
    FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);

    if (fileTexture && !fileTexture->GetUserDataPtr()) {
      const FbxString fileName = fileTexture->GetFileName();
    }
  }
}

void LoadTexture(FbxScene* pScene, const char* pFbxFileName, std::vector<Texture> texture) {}

bool TransformFbxScene(FbxManager* fbxSdkManager, FbxImporter* fbxImporter, FbxScene* fbxScene, Flag64 flags, Scene*& resScene,
                       std::vector<std::string>& resErrorInfos) {
  FbxStatus status;
  FbxArray<FbxString*> details;
  FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(fbxScene), &status, &details);
  bool notify = (!sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData) && details.GetCount() > 0) ||
                (fbxImporter->GetStatus().GetCode() != FbxStatus::eSuccess);

  if (notify) {
    if (details.GetCount()) {
      std::string errorString = "Scene integrity verification failed with the following errors:\n";
      for (int i = 0; i < details.GetCount(); i++) {
        errorString += std::string(details[i]->Buffer());
        errorString += "\n";
      }
      resErrorInfos.push_back(std::move(errorString));
    }

    if (fbxImporter->GetStatus().GetCode() != FbxStatus::eSuccess) {
      std::string errorString = "WARNING:\n";
      errorString += "   The importer was able to read the file but with errors.\n";
      errorString += "   Loaded scene may be incomplete.\n\n";
      errorString += "   Last error message:'%s'\n";
      errorString += std::string(fbxImporter->GetStatus().GetErrorString());
      resErrorInfos.push_back(std::move(errorString));
    }

    return false;
  }

  FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
  FbxAxisSystem targetAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);

  bool useLeftHanded = CheckFlag(flags, long(SceneLoadOption::UseDirect12));
  if (useLeftHanded) {
    targetAxisSystem = FbxAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded);
  }

  if (sceneAxisSystem != targetAxisSystem) {
    targetAxisSystem.ConvertScene(fbxScene);
  }

  FbxSystemUnit sceneSystemUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
  if (sceneSystemUnit.GetScaleFactor() != 1.0) {
    FbxSystemUnit::cm.ConvertScene(fbxScene);
  }

  FbxArray<FbxString*> animStackNameArray;
  fbxScene->FillAnimStackNameArray(animStackNameArray);

  FbxArray<FbxNode*> cameraArray;
  FillCameraArray(fbxScene, cameraArray);

  std::vector<Texture> textureArray;

  // FbxArray<FbxPose*> poseArray;

  FbxGeometryConverter geomConverter(fbxSdkManager);
  try {
    geomConverter.Triangulate(fbxScene, /*replace*/ true);
  } catch (std::runtime_error) {
    resErrorInfos.push_back(std::string("Scene integrity verification failed."));
    return false;
  }

  return true;
}

void FillCameraArrayRecursive(FbxNode* fbxNode, FbxArray<FbxNode*>& resCameraArray) {
  if (fbxNode) {
    if (fbxNode->GetNodeAttribute()) {
      if (fbxNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eCamera) {
        resCameraArray.Add(fbxNode);
      }
    }

    const int lCount = fbxNode->GetChildCount();
    for (int i = 0; i < lCount; i++) {
      FillCameraArrayRecursive(fbxNode->GetChild(i), resCameraArray);
    }
  }
}

void FillCameraArray(FbxScene* fbxScene, FbxArray<FbxNode*>& resCameraArray) {
  resCameraArray.Clear();

  FillCameraArrayRecursive(fbxScene->GetRootNode(), resCameraArray);
}

}  // namespace RE::Core