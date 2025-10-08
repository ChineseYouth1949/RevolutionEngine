#include "FirstApp.h"

namespace FirstApp {

int RunExample() {
  FbxManager* lSdkManager = FbxManager::Create();

  FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
  lSdkManager->SetIOSettings(ios);

  FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

  if (!lImporter->Initialize(gLoadFilename, -1, lSdkManager->GetIOSettings())) {
    std::cout << "Call to FbxImporter::Initialize() failed." << std::endl;
    std::cout << "Error returned: " << lImporter->GetStatus().GetErrorString() << std::endl;
    exit(-1);
  }

  FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
  lImporter->Import(lScene);
  lImporter->Destroy();

  FbxNode* lRootNode = lScene->GetRootNode();
  if (lRootNode) {
    for (int i = 0; i < lRootNode->GetChildCount(); i++) {
      PrintNode(lRootNode->GetChild(i));
    }
  }

  lSdkManager->Destroy();

  return 0;
}

void PrintNode(FbxNode* pNode) {
  const char* nodeName = pNode->GetName();

  FbxDouble3 translation = pNode->LclTranslation.Get();
  FbxDouble3 rotation = pNode->LclRotation.Get();
  FbxDouble3 scaling = pNode->LclScaling.Get();

  printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n", nodeName, translation[0], translation[1],
         translation[2], rotation[0], rotation[1], rotation[2], scaling[0], scaling[1], scaling[2]);

  for (size_t i = 0; i < pNode->GetNodeAttributeCount(); i++) {
    PrintAttribute(pNode->GetNodeAttributeByIndex(i));
  }

  for (size_t i = 0; i < pNode->GetChildCount(); i++) {
    PrintNode(pNode->GetChild(i));
  }

  numTabs--;
  PrintTabs();
  std::cout << "</node>" << std::endl;
}

void PrintTabs() {
  for (int i = 0; i < numTabs; i++) {
    std::cout << "\t";
  }
}

void PrintAttribute(FbxNodeAttribute* pAttribute) {
  if (!pAttribute)
    return;

  FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
  FbxString attrName = pAttribute->GetName();
  PrintTabs();

  printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}

FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
  switch (type) {
    case FbxNodeAttribute::eUnknown:
      return "unidentified";
    case FbxNodeAttribute::eNull:
      return "null";
    case FbxNodeAttribute::eMarker:
      return "marker";
    case FbxNodeAttribute::eSkeleton:
      return "skeleton";
    case FbxNodeAttribute::eMesh:
      return "mesh";
    case FbxNodeAttribute::eNurbs:
      return "nurbs";
    case FbxNodeAttribute::ePatch:
      return "patch";
    case FbxNodeAttribute::eCamera:
      return "camera";
    case FbxNodeAttribute::eCameraStereo:
      return "stereo";
    case FbxNodeAttribute::eCameraSwitcher:
      return "camera switcher";
    case FbxNodeAttribute::eLight:
      return "light";
    case FbxNodeAttribute::eOpticalReference:
      return "optical reference";
    case FbxNodeAttribute::eOpticalMarker:
      return "marker";
    case FbxNodeAttribute::eNurbsCurve:
      return "nurbs curve";
    case FbxNodeAttribute::eTrimNurbsSurface:
      return "trim nurbs surface";
    case FbxNodeAttribute::eBoundary:
      return "boundary";
    case FbxNodeAttribute::eNurbsSurface:
      return "nurbs surface";
    case FbxNodeAttribute::eShape:
      return "shape";
    case FbxNodeAttribute::eLODGroup:
      return "lodgroup";
    case FbxNodeAttribute::eSubDiv:
      return "subdiv";
    default:
      return "unknown";
  }
}

}  // namespace FirstApp