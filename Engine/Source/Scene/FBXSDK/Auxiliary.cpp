#include "Auxiliary.h"

#include <mutex>
#include <filesystem>

#include "Base/Base.h"
#include "Memory/Memory.h"

#include "Scene/SceneObj.h"
#include "Scene/ISceneImporter.h"

namespace RE::Core {

void FBXSdkInit() {
  static std::mutex sMutex;
  static bool sInited = false;

  std::lock_guard lock(sMutex);

  if (!sInited) {
    FbxSetMallocHandler(GMalloc);
    FbxSetCallocHandler(GCalloc);
    FbxSetReallocHandler(GRealloc);
    FbxSetFreeHandler(GFree);

    sInited = true;
  }
}

void FillCameraArray(FbxScene* fbxScene, FbxArray<FbxNode*>& resCameraArray);

void LoadTexture(FbxScene* fbxScene, const char* pFbxFileName, std::vector<Texture>& resTextures);

bool FBXSceneTransform(FbxManager* fbxSdkManager, FbxImporter* fbxImporter, FbxScene* fbxScene, Flag64 flags, Scene*& resScene,
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

  bool useLeftHanded = CheckFlagExist(flags, long(SceneLoadOption::UseDirect12));
  if (useLeftHanded) {
    targetAxisSystem = FbxAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded);
  }

  if (sceneAxisSystem != targetAxisSystem) {
    targetAxisSystem.ConvertScene(fbxScene);
  }

  FbxSystemUnit sceneSystemUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
  if (sceneSystemUnit.GetScaleFactor() != 1.0) {
    FbxSystemUnit::m.ConvertScene(fbxScene);
  }

  FbxArray<FbxString*> animStackNameArray;
  fbxScene->FillAnimStackNameArray(animStackNameArray);

  FbxArray<FbxNode*> cameraArray;
  FillCameraArray(fbxScene, cameraArray);

  FbxGeometryConverter geomConverter(fbxSdkManager);
  try {
    geomConverter.Triangulate(fbxScene, /*replace*/ true);
  } catch (std::runtime_error) {
    resErrorInfos.push_back(std::string("Scene integrity verification failed."));
    return false;
  }

  std::vector<Texture> textureArray;

  // FbxArray<FbxPose*> poseArray;

  return true;
}

void FillCameraArrayRecursive(FbxNode* fbxNode, FbxArray<FbxNode*>& resCameraArray);

void FillCameraArray(FbxScene* fbxScene, FbxArray<FbxNode*>& resCameraArray) {
  resCameraArray.Clear();
  FillCameraArrayRecursive(fbxScene->GetRootNode(), resCameraArray);
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

bool FileExist(std::string filename);
bool ReadTexture(FbxFileTexture* pFileTexture, const char* pFbxFileName, std::string& pResTextureFile, std::string& pResErrorInfo);

void LoadTexture(FbxScene* pFbxScene, const char* pFbxFileName, std::vector<Texture>& pResTextures, std::vector<std::string>& pResErrorInfos) {
  const int lTextureCount = pFbxScene->GetTextureCount();

  std::string lResTextureFileName;
  std::string lResErrorInfo;

  for (int lTextureIndex = 0; lTextureIndex < lTextureCount; lTextureIndex++) {
    FbxTexture* lTexture = pFbxScene->GetTexture(lTextureIndex);
    FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture);

    if (lFileTexture && !lFileTexture->GetUserDataPtr()) {
      bool exist = ReadTexture(lFileTexture, pFbxFileName, lResTextureFileName, lResErrorInfo);
      if (exist) {
        pResTextures.push_back(Texture(lResTextureFileName));
      } else {
        pResErrorInfos.push_back(std::move(lResErrorInfo));
      }

      lFileTexture->SetUserDataPtr(pFbxScene);
    }
  }
}

bool FileIsExist(const std::string& filename) {
  return std::filesystem::exists(filename);
}
bool ReadTexture(FbxFileTexture* pFileTexture, const char* pFbxFileName, std::string& pResTextureFile, std::string& pResErrorInfo) {
  const FbxString lFileName = pFileTexture->GetFileName();

  bool lStatus = FileIsExist(lFileName.Buffer());
  if (lStatus) {
    pResTextureFile = std::string(lFileName.Buffer());
    return true;
  }

  const FbxString lAbsFbxFileName = FbxPathUtils::Resolve(pFbxFileName);
  const FbxString lAbsFolderName = FbxPathUtils::GetFolderName(lAbsFbxFileName);

  const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, pFileTexture->GetRelativeFileName());
  lStatus = FileIsExist(lResolvedFileName.Buffer());
  if (lStatus) {
    pResTextureFile = std::string(lResolvedFileName.Buffer());
    return true;
  }

  const FbxString lTextureFileName = FbxPathUtils::GetFileName(lFileName);
  const FbxString lResolvedFileName2 = FbxPathUtils::Bind(lAbsFolderName, lTextureFileName);
  lStatus = FileIsExist(lResolvedFileName2.Buffer());
  if (lStatus) {
    pResTextureFile = std::string(lResolvedFileName2.Buffer());
    return true;
  }

  pResErrorInfo = std::string("Failed to load texture file: ") + std::string(lFileName.Buffer());
  return false;
}

}  // namespace RE::Core