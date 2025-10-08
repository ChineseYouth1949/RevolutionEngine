#include "FBX_Utility.h"

namespace SceneTreeView {

#ifdef IOS_REF
#undef IOS_REF
#define IOS_REF (*(pSdkManager->GetIOSettings()))
#endif

FbxUtil::FbxUtil() {}
FbxUtil::~FbxUtil() {
  DestroySdkObjects(mSdkManager);
}

FbxUtil& FbxUtil::Instance() {
  static FbxUtil instance;
  return instance;
}

void FbxUtil::InitializeSdkManagerAndScene() {
  // 创建 FBX SDK 内存管理器对象。
  // SDK 管理器负责为 SDK 中的几乎所有类分配和释放内存。
  mSdkManager = FbxManager::Create();

  // 创建一个 IOSettings 对象
  FbxIOSettings* ios = FbxIOSettings::Create(mSdkManager, IOSROOT);
  mSdkManager->SetIOSettings(ios);

  // 创建场景对象
  mScene = FbxScene::Create(mSdkManager, "myScene");
}

bool FbxUtil::LoadFBXScene(const char* pFbxFilePath) {
  if (LoadScene(mSdkManager, mScene, pFbxFilePath) == false)
    return false;

  return true;
}

const char* FbxUtil::GetReaderOFNFilters() {
  int nbReaders = mSdkManager->GetIOPluginRegistry()->GetReaderFormatCount();

  FbxString s;
  int i = 0;

  for (i = 0; i < nbReaders; i++) {
    s += mSdkManager->GetIOPluginRegistry()->GetReaderFormatDescription(i);
    s += "|*.";
    s += mSdkManager->GetIOPluginRegistry()->GetReaderFormatExtension(i);
    s += "|";
  }

  // replace | by \0
  int nbChar = int(strlen(s.Buffer())) + 1;
  char* filter = new char[nbChar];
  memset(filter, 0, nbChar);

  FBXSDK_strcpy(filter, nbChar, s.Buffer());

  for (i = 0; i < int(strlen(s.Buffer())); i++) {
    if (filter[i] == '|') {
      filter[i] = 0;
    }
  }

  // the caller must delete this allocated memory
  return filter;
}

const FbxNode* FbxUtil::GetRootNode() {
  return mScene->GetRootNode();
}

// to get the root node name
const char* FbxUtil::GetRootNodeName() {
  return GetRootNode()->GetName();
}

FbxString FbxUtil::GetNodeNameAndAttributeTypeName(const FbxNode* pNode) {
  FbxString s = pNode->GetName();

  FbxNodeAttribute::EType lAttributeType;

  if (pNode->GetNodeAttribute() == NULL) {
    s += " (No node attribute type)";
  } else {
    lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

    switch (lAttributeType) {
      case FbxNodeAttribute::eMarker:
        s += " (Marker)";
        break;
      case FbxNodeAttribute::eSkeleton:
        s += " (Skeleton)";
        break;
      case FbxNodeAttribute::eMesh:
        s += " (Mesh)";
        break;
      case FbxNodeAttribute::eCamera:
        s += " (Camera)";
        break;
      case FbxNodeAttribute::eLight:
        s += " (Light)";
        break;
      case FbxNodeAttribute::eBoundary:
        s += " (Boundary)";
        break;
      case FbxNodeAttribute::eOpticalMarker:
        s += " (Optical marker)";
        break;
      case FbxNodeAttribute::eOpticalReference:
        s += " (Optical reference)";
        break;
      case FbxNodeAttribute::eCameraSwitcher:
        s += " (Camera switcher)";
        break;
      case FbxNodeAttribute::eNull:
        s += " (Null)";
        break;
      case FbxNodeAttribute::ePatch:
        s += " (Patch)";
        break;
      case FbxNodeAttribute::eNurbs:
        s += " (NURB)";
        break;
      case FbxNodeAttribute::eNurbsSurface:
        s += " (Nurbs surface)";
        break;
      case FbxNodeAttribute::eNurbsCurve:
        s += " (NURBS curve)";
        break;
      case FbxNodeAttribute::eTrimNurbsSurface:
        s += " (Trim nurbs surface)";
        break;
      case FbxNodeAttribute::eUnknown:
        s += " (Unidentified)";
        break;
    }
  }

  return s;
}

// to get a string from the node default translation values
FbxString FbxUtil::GetDefTranslationInfo(const FbxNode* pNode) {
  FbxVector4 v4;
  v4 = ((FbxNode*)pNode)->LclTranslation.Get();

  return FbxString("Translation (X,Y,Z): ") + FbxString(v4[0]) + ", " + FbxString(v4[1]) + ", " + FbxString(v4[2]);
}
FbxString FbxUtil::GetDefRotationInfo(const FbxNode* pNode) {
  FbxVector4 v4;
  v4 = ((FbxNode*)pNode)->LclRotation.Get();

  return FbxString("Rotation (X,Y,Z): ") + FbxString(v4[0]) + ", " + FbxString(v4[1]) + ", " + FbxString(v4[2]);
}
FbxString FbxUtil::GetDefScaleInfo(const FbxNode* pNode) {
  FbxVector4 v4;
  v4 = ((FbxNode*)pNode)->LclScaling.Get();

  return FbxString("Scale (X,Y,Z): ") + FbxString(v4[0]) + ", " + FbxString(v4[1]) + ", " + FbxString(v4[2]);
}

// to get a string from the node visibility value
FbxString FbxUtil::GetNodeVisibility(const FbxNode* pNode) {
  return FbxString("Visibility: ") + (pNode->GetVisibility() ? "Yes" : "No");
}

void FbxUtil::DestroySdkObjects(FbxManager* pSdkManager) {
  if (pSdkManager) {

    pSdkManager->Destroy();
  }
}
bool FbxUtil::LoadScene(FbxManager* pSdkManager, FbxScene* pScene, const char* pFbxFilePath) {
  bool lStatus;

  // Create an importer.
  FbxImporter* lImporter = FbxImporter::Create(pSdkManager, "");

  // Initialize the importer by providing a filename.
  bool lImportStatus = lImporter->Initialize(pFbxFilePath, -1, pSdkManager->GetIOSettings());

  if (!lImportStatus) {
    // Destroy the importer
    lImporter->Destroy();
    return false;
  }

  if (lImporter->IsFBX()) {
    // Set the import states. By default, the import states are always set to
    // true. The code below shows how to change these states.
    IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
    IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
    IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
    IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
    IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
    IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
    IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
  }

  // Import the scene
  lStatus = lImporter->Import(pScene);

  // Destroy the importer
  lImporter->Destroy();

  return lStatus;
}

}  // namespace SceneTreeView