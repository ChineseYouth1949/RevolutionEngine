#pragma once

#include "Base/FlagType.h"

#include <string>
#include <vector>

#include <fbxsdk.h>

namespace RE::Core {
class Scene;
class Texture;
class Material;

void FBXSDKInit();

class FbxSceneConverter {
 public:
  FbxSceneConverter();
  ~FbxSceneConverter();

  bool Transform(FbxManager* pFbxSdkManager, FbxImporter* pFbxImporter, std::string pFbxFileName, FbxScene* pFbxScene, Flag64 pFalgs);
  Scene* GetScene(bool pRemove = true);
  std::vector<std::string>& GetErrorInfos();

 protected:
  void Reset();

  bool CheckScene();
  void ConvertAxisAndUnit();
  bool ConvertPolygonToTriangle();

  void LoadAnimation();
  void LoadCamera();
  void LoadTexture();
  void LoadMaterial();

  static FbxArray<FbxNode*> FillCameraArray(FbxScene* pFbxScene);
  static void FillCameraArrayImpl(FbxNode* pFbxNode, FbxArray<FbxNode*>& pResCameraArray);

  static bool FindTexture(FbxFileTexture* pFileTexture, const std::string& pFbxFileName, std::string& pResTextureFile);
  static bool FileExist(const std::string& pFilename);

  static std::vector<Material*> FindMaterial(FbxScene* pFbxScene);
  static void FindMaterialImpl(FbxNode* pFbxNode, std::vector<Material>& pResMaterials);

 private:
  std::string mFbxFileName;
  FbxManager* mFbxSdkManager{nullptr};
  FbxImporter* mFbxImporter{nullptr};
  FbxScene* mFbxScene{nullptr};
  Flag64 mFalgs;

  Scene* mScene{nullptr};
  std::vector<std::string> mErrorInfos;
};

}  // namespace RE::Core