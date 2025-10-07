#pragma once

#include <fbxsdk.h>

#include <string>
#include <iostream>

namespace LoadAndSave {

const char* gLoadFilename = "./Assets/Model/mech-drone/source/Drone.FBX";
const char* gSaveFilename = "./Assets/Model/mech-drone/source/Drone-Copy.FBX";

bool LoadScene(FbxManager* pSdkManager, FbxScene* pScene, const char* pFilename);
bool SaveScene(FbxManager* pSdkManager, FbxScene* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia);

int RunExample() {
  FbxManager* lSdkManager = FbxManager::Create();

  FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
  lSdkManager->SetIOSettings(ios);

  FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

  bool r = LoadScene(lSdkManager, lScene, gLoadFilename);
  if (!r) {
    lScene->Destroy();
    lSdkManager->Destroy();
    return -1;
  }

  r = SaveScene(lSdkManager, lScene, gSaveFilename, 0, false);

  lScene->Destroy();
  lSdkManager->Destroy();

  return 0;
}

bool LoadScene(FbxManager* pSdkManager, FbxScene* pScene, const char* pFilename) {
  int lSDKMajor, lSDKMinor, lSDKRevision;
  FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

  FbxImporter* lImporter = FbxImporter::Create(pSdkManager, "myImporter");

  const bool lImportStatus = lImporter->Initialize(pFilename, -1, pSdkManager->GetIOSettings());

  int lFileMajor, lFileMinor, lFileRevision;
  lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

  if (!lImportStatus) {
    FbxString error = lImporter->GetStatus().GetErrorString();

    if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion) {
      printf("FBX version number for this FBX SDK is %d.%d.%d", lSDKMajor, lSDKMinor, lSDKRevision);
      printf("FBX version number for file %s is %d.%d.%d", pFilename, lFileMajor, lFileMinor, lFileRevision);
    }

    return false;
  }

  printf("FBX version number for this FBX SDK is %d.%d.%d", lSDKMajor, lSDKMinor, lSDKRevision);

  if (lImporter->IsFBX()) {
    printf("FBX version number for file %s is %d.%d.%d", pFilename, lFileMajor, lFileMinor, lFileRevision);

    printf("Animation Stack Information");
    int lAnimStackCount = lImporter->GetAnimStackCount();

    printf("    Number of animation stacks: %d", lAnimStackCount);
    printf("    Active animation stack: \"%s\"", lImporter->GetActiveAnimStackName().Buffer());

    for (int i = 0; i < lAnimStackCount; i++) {
      FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

      printf("    Animation Stack %d", i);
      printf("         Name: \"%s\"", lTakeInfo->mName.Buffer());
      printf("         Description: \"%s\"", lTakeInfo->mDescription.Buffer());

      // Change the value of the import name if the animation stack should
      // be imported under a different name.
      printf("         Import Name: \"%s\"", lTakeInfo->mImportName.Buffer());

      // Set the value of the import state to false
      // if the animation stack should be not be imported.
      printf("         Import State: %s", lTakeInfo->mSelect ? "true" : "false");
    }

    FbxIOSettings* manager_ios = pSdkManager->GetIOSettings();
    manager_ios->SetBoolProp(IMP_FBX_MATERIAL, true);
    manager_ios->SetBoolProp(IMP_FBX_TEXTURE, true);
    manager_ios->SetBoolProp(IMP_FBX_LINK, true);
    manager_ios->SetBoolProp(IMP_FBX_SHAPE, true);
    manager_ios->SetBoolProp(IMP_FBX_GOBO, true);
    manager_ios->SetBoolProp(IMP_FBX_ANIMATION, true);
    manager_ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
  }

  bool lStatus = lImporter->Import(pScene);

  if (lStatus == false &&  // The import file may have a password
      lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError) {
    printf("Please enter password: ");

    FbxString lString("\0");

    FbxIOSettings* manager_ios = pSdkManager->GetIOSettings();
    manager_ios->SetStringProp(IMP_FBX_PASSWORD, lString);
    manager_ios->SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

    lStatus = lImporter->Import(pScene);

    if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError) {
      printf("Incorrect password: file not imported.");
    }
  }

  // Destroy the importer
  lImporter->Destroy();

  return lStatus;
}

bool SaveScene(FbxManager* pSdkManager, FbxScene* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia) {
  // Create an exporter.
  FbxExporter* lExporter = FbxExporter::Create(pSdkManager, "");

  if (pFileFormat < 0 || pFileFormat >= pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount()) {
    // Write in fall back format if pEmbedMedia is true
    pFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();

    if (!pEmbedMedia) {
      //Try to export in ASCII if possible
      int lFormatIndex, lFormatCount = pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount();

      for (lFormatIndex = 0; lFormatIndex < lFormatCount; lFormatIndex++) {
        if (pSdkManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex)) {
          FbxString lDesc = pSdkManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
          if (lDesc.Find("ascii") >= 0) {
            pFileFormat = lFormatIndex;
            break;
          }
        }
      }
    }
  }

  // Initialize the exporter by providing a Filename.
  if (lExporter->Initialize(pFilename, pFileFormat, pSdkManager->GetIOSettings()) == false) {
    printf("Call to FbxExporter::Initialize() failed.");
    printf("Error returned: %s", lExporter->GetStatus().GetErrorString());
    return false;
  }

  int lMajor, lMinor, lRevision;
  bool lStatus = true;

  FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
  printf("FBX version number for this FBX SDK is %d.%d.%d", lMajor, lMinor, lRevision);

  if (pSdkManager->GetIOPluginRegistry()->WriterIsFBX(pFileFormat)) {
    // Export options determine what kind of data is to be imported.
    // The default (except for the option eEXPORT_TEXTURE_AS_EMBEDDED)
    // is true, but here we set the options explicitly.
    FbxIOSettings* manager_ios = pSdkManager->GetIOSettings();

    manager_ios->SetBoolProp(EXP_FBX_MATERIAL, true);
    manager_ios->SetBoolProp(EXP_FBX_TEXTURE, true);
    manager_ios->SetBoolProp(EXP_FBX_EMBEDDED, pEmbedMedia);
    manager_ios->SetBoolProp(EXP_FBX_SHAPE, true);
    manager_ios->SetBoolProp(EXP_FBX_GOBO, true);
    manager_ios->SetBoolProp(EXP_FBX_ANIMATION, true);
    manager_ios->SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);
  }

  // Export the scene.
  lStatus = lExporter->Export(pScene);

  // Destroy the exporter.
  lExporter->Destroy();

  return lStatus;
}

}  // namespace LoadAndSave