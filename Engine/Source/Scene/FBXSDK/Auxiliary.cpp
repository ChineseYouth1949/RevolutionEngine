#include "Auxiliary.h"

#include <mutex>
#include <filesystem>

#include "Base/Base.h"
#include "Memory/Memory.h"

#include "Scene/SceneObj.h"
#include "Scene/Texture.h"
#include "Scene/ISceneImporter.h"

namespace RE::Core {

void FBXSDKInit() {
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

FbxSceneConverter::FbxSceneConverter() {}
FbxSceneConverter::~FbxSceneConverter() {
  Reset();
}

void FbxSceneConverter::Reset() {
  mErrorInfos.clear();

  if (mScene) {
    mScene->Destroy();
    delete mScene;
    mScene = nullptr;
  }
}

Scene* FbxSceneConverter::GetScene(bool pRemove) {
  Scene* lResScene = nullptr;

  if (mScene) {
    lResScene = mScene;

    if (pRemove) {
      mScene = nullptr;
    }
  }

  return lResScene;
}

std::vector<std::string>& FbxSceneConverter::GetErrorInfos() {
  return mErrorInfos;
}

bool FbxSceneConverter::Transform(FbxManager* pFbxSdkManager, FbxImporter* pFbxImporter, std::string pFbxFileName, FbxScene* pFbxScene,
                                  Flag64 pFalgs) {
  // init obj data
  {
    Reset();

    mFbxFileName = pFbxFileName;
    mFbxSdkManager = pFbxSdkManager;
    mFbxImporter = pFbxImporter;
    mFbxScene = pFbxScene;
    mFalgs = pFalgs;
  }

  // Check scene
  if (!CheckScene()) {
    return false;
  }

  // Convert axis and unit
  ConvertAxisAndUnit();

  if (!ConvertPolygonToTriangle()) {
    return false;
  }

  // allocate scene obj
  mScene = GAllocateConstructor<Scene>();

  LoadAnimation();
  LoadCamera();
  LoadTexture();
  LoadMaterial();

  return true;
}

bool FbxSceneConverter::CheckScene() {
  FbxStatus lStatus;
  FbxArray<FbxString*> lDetails;
  FbxSceneCheckUtility lSceneCheck(FbxCast<FbxScene>(mFbxScene), &lStatus, &lDetails);
  bool lNotify = (!lSceneCheck.Validate(FbxSceneCheckUtility::eCkeckData) && lDetails.GetCount() > 0) ||
                 (mFbxImporter->GetStatus().GetCode() != FbxStatus::eSuccess);

  if (lNotify) {
    if (lDetails.GetCount()) {
      std::string lErrorString = "Scene integrity verification failed with the following errors:\n";
      for (int i = 0; i < lDetails.GetCount(); i++) {
        lErrorString += std::string(lDetails[i]->Buffer());
        lErrorString += "\n";
      }
      mErrorInfos.push_back(std::move(lErrorString));
    }

    if (mFbxImporter->GetStatus().GetCode() != FbxStatus::eSuccess) {
      std::string lErrorString = "WARNING:\n";
      lErrorString += "   The importer was able to read the file but with errors.\n";
      lErrorString += "   Loaded scene may be incomplete.\n\n";
      lErrorString += "   Last error message:'%s'\n";
      lErrorString += std::string(mFbxImporter->GetStatus().GetErrorString());
      mErrorInfos.push_back(std::move(lErrorString));
    }

    return false;
  }

  return true;
}

void FbxSceneConverter::ConvertAxisAndUnit() {
  FbxAxisSystem lSceneAxisSystem = mFbxScene->GetGlobalSettings().GetAxisSystem();
  FbxAxisSystem lTargetAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);

  bool lUseLeftHanded = CheckFlagExist(mFalgs, long(SceneLoadOption::UseDirect12));
  if (lUseLeftHanded) {
    lTargetAxisSystem = FbxAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded);
  }

  if (lSceneAxisSystem != lTargetAxisSystem) {
    lTargetAxisSystem.ConvertScene(mFbxScene);
  }

  FbxSystemUnit lSceneSystemUnit = mFbxScene->GetGlobalSettings().GetSystemUnit();
  if (lSceneSystemUnit.GetScaleFactor() != 1.0) {
    FbxSystemUnit::m.ConvertScene(mFbxScene);
  }
}

bool FbxSceneConverter::ConvertPolygonToTriangle() {
  FbxGeometryConverter lGeomConverter(mFbxSdkManager);
  try {
    lGeomConverter.Triangulate(mFbxScene, /*replace*/ true);
  } catch (std::runtime_error) {
    mErrorInfos.push_back(std::string("Scene integrity verification failed."));
    return false;
  }

  return true;
}

void FbxSceneConverter::LoadAnimation() {
  FbxArray<FbxString*> lAnimStackNameArray;
  mFbxScene->FillAnimStackNameArray(lAnimStackNameArray);

  for (size_t i = 0; i < lAnimStackNameArray.Size(); i++) {
    mScene->mAnimStackNames.push_back(lAnimStackNameArray[i]->Buffer());
  }
}

void FbxSceneConverter::LoadCamera() {
  FbxArray<FbxNode*> lFbxCameraArray = FillCameraArray(mFbxScene);
}

void FbxSceneConverter::LoadTexture() {
  const int lTextureCount = mFbxScene->GetTextureCount();

  std::string lResTextureFileName;
  std::string lResErrorInfo;

  std::vector<Texture> lTextures;

  for (int lTextureIndex = 0; lTextureIndex < lTextureCount; lTextureIndex++) {
    FbxTexture* lTexture = mFbxScene->GetTexture(lTextureIndex);
    FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture);

    if (lFileTexture && !lFileTexture->GetUserDataPtr()) {
      bool exist = FindTexture(lFileTexture, mFbxFileName.c_str(), lResTextureFileName);
      if (exist) {
        lTextures.push_back(Texture(lResTextureFileName));
      } else {
        std::string lErrorInfo = std::string("Not found texture : ") + lResTextureFileName;
        mErrorInfos.push_back(std::move(lErrorInfo));
      }

      lFileTexture->SetUserDataPtr(mFbxScene);
    }
  }
}

void FbxSceneConverter::LoadMaterial() {}

FbxArray<FbxNode*> FbxSceneConverter::FillCameraArray(FbxScene* pFbxScene) {
  FbxArray<FbxNode*> lResCameraArray;
  FillCameraArrayImpl(pFbxScene->GetRootNode(), lResCameraArray);
  return lResCameraArray;
}
void FbxSceneConverter::FillCameraArrayImpl(FbxNode* pFbxNode, FbxArray<FbxNode*>& pResCameraArray) {
  if (pFbxNode) {
    if (pFbxNode->GetNodeAttribute()) {
      if (pFbxNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eCamera) {
        pResCameraArray.Add(pFbxNode);
      }
    }

    const int lCount = pFbxNode->GetChildCount();
    for (int i = 0; i < lCount; i++) {
      FillCameraArrayImpl(pFbxNode->GetChild(i), pResCameraArray);
    }
  }
}

bool FbxSceneConverter::FindTexture(FbxFileTexture* pFileTexture, const std::string& pFbxFileName, std::string& pResTextureFile) {
  const FbxString lFileName = pFileTexture->GetFileName();

  bool lStatus = FileExist(lFileName.Buffer());
  if (lStatus) {
    pResTextureFile = std::string(lFileName.Buffer());
    return true;
  }

  const FbxString lAbsFbxFileName = FbxPathUtils::Resolve(pFbxFileName.c_str());
  const FbxString lAbsFolderName = FbxPathUtils::GetFolderName(lAbsFbxFileName);

  const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, pFileTexture->GetRelativeFileName());
  lStatus = FileExist(lResolvedFileName.Buffer());
  if (lStatus) {
    pResTextureFile = std::string(lResolvedFileName.Buffer());
    return true;
  }

  const FbxString lTextureFileName = FbxPathUtils::GetFileName(lFileName);
  const FbxString lResolvedFileName2 = FbxPathUtils::Bind(lAbsFolderName, lTextureFileName);
  lStatus = FileExist(lResolvedFileName2.Buffer());
  if (lStatus) {
    pResTextureFile = std::string(lResolvedFileName2.Buffer());
    return true;
  }

  pResTextureFile = std::string(lFileName.Buffer());
  return false;
}
bool FbxSceneConverter::FileExist(const std::string& pFilename) {
  return std::filesystem::exists(pFilename);
}

std::vector<Material*> FbxSceneConverter::FindMaterial(FbxScene* pFbxScene) {
  std::vector<Material*> lResMaterials;

  return lResMaterials;
}
void FbxSceneConverter::FindMaterialImpl(FbxNode* pFbxNode, std::vector<Material>& pResMaterials) {}

}  // namespace RE::Core