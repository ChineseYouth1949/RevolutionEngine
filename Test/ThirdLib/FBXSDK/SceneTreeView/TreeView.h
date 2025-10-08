#pragma once

#include <windows.h>
#include <commctrl.h>  // for treeview control

#include <fbxsdk.h>

namespace SceneTreeView {

HTREEITEM InsertTreeViewItem(const HWND hTv, const char* txt, HTREEITEM htiParent);
void Add_TreeViewItem_KFbxNode_Parameters(const FbxNode* pNode, HWND hTv, HTREEITEM htiParent);

}  // namespace SceneTreeView