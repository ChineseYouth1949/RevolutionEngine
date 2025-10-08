#include "TreeView.h"

#include "FBX_Utility.h"

namespace SceneTreeView {

// used to add a new treeview item
HTREEITEM InsertTreeViewItem(const HWND hTv, const char* txt, HTREEITEM htiParent) {
  TVITEM tvi = {0};
  tvi.mask = TVIF_TEXT | TVIF_PARAM;               // text + param only
  tvi.pszText = (LPSTR)txt;                        // caption
  tvi.cchTextMax = static_cast<int>(strlen(txt));  // length of item label

  TVINSERTSTRUCT tvis = {0};
  tvis.item = tvi;
  tvis.hInsertAfter = 0;
  tvis.hParent = htiParent;  // parent item of item to be inserted

  return reinterpret_cast<HTREEITEM>(SendMessage(hTv, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvis)));
}

// used to add FbxNode parameters
void Add_TreeViewItem_KFbxNode_Parameters(const FbxNode* pNode, HWND hTv, HTREEITEM htiParent) {
  if (pNode == NULL)
    return;

  // show node default translation
  InsertTreeViewItem(hTv, FbxUtil::GetDefaultTranslationInfo(pNode).Buffer(), htiParent);

  // show node visibility
  InsertTreeViewItem(hTv, FbxUtil::GetNodeVisibility(pNode).Buffer(), htiParent);
}

}  // namespace SceneTreeView