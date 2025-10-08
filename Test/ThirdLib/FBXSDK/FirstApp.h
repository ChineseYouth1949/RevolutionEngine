#pragma once

#include <fbxsdk.h>

#include <string>
#include <iostream>

namespace FirstApp {

static const char* gLoadFilename = "./Assets/Model/mech-drone/source/Drone.FBX";

static int numTabs = 0;
void PrintTabs();

FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
void PrintAttribute(FbxNodeAttribute* pAttribute);
void PrintNode(FbxNode* pNode);

int RunExample();
}