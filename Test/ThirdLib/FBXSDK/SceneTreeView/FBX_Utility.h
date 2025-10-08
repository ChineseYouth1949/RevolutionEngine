#pragma once

#include <fbxsdk.h>

namespace SceneTreeView {

class FbxUtil {
 public:
  static FbxUtil& Instance();

  void InitializeSdkManagerAndScene();
  bool LoadFBXScene(const char* pFbxFilePath);

  const char* GetReaderOFNFilters();

  const FbxNode* GetRootNode();
  const char* GetRootNodeName();

  static FbxString GetNodeNameAndAttributeTypeName(const FbxNode* pNode);
  static FbxString GetDefaultTranslationInfo(const FbxNode* pNode);
  static FbxString GetNodeVisibility(const FbxNode* pNode);

 protected:
  static void DestroySdkObjects(FbxManager* pSdkManager);
  static bool LoadScene(FbxManager* pSdkManager, FbxScene* pScene, const char* pFbxFilePath);

 private:
  FbxUtil();
  ~FbxUtil();

  FbxManager* mSdkManager = NULL;
  FbxScene* mScene = NULL;
};

}  // namespace SceneTreeView