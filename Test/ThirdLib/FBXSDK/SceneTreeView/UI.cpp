#include "UI.h"

#include "FBX_Utility.h"
#include "TreeView.h"

#include <string>
#include <iostream>
#include <tchar.h>

namespace SceneTreeView {

#define MAX_LOADSTRING 100
TCHAR szWindowClass[MAX_LOADSTRING] = _T("My Window Class");  // 全局注册窗口类名

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 注册窗口类到szWindowClass
ATOM UIRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEX wcex = {0};

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.hInstance = hInstance;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
  wcex.lpszClassName = szWindowClass;

  return RegisterClassEx(&wcex);
}

HINSTANCE hInst;    // 全局程序实例
HWND ghWnd = NULL;  // 全局窗口句柄

// 使用szWindowClass创建窗口
BOOL InitWindow(HINSTANCE hInstance, int nCmdShow) {
  hInst = hInstance;

  ghWnd = CreateWindow(szWindowClass,                           // LPCTSTR lpClassName
                       "FBX SDK SceneTreeView sample program",  // LPCTSTR lpWindowName
                       WS_OVERLAPPED | WS_SYSMENU,              // DWORD dwStyle
                       400,                                     // int x
                       200,                                     // int Y
                       700,                                     // int nWidth
                       800,                                     // int nHeight
                       NULL,                                    // HWND hWndParent
                       NULL,                                    // HMENU hMenu
                       hInstance,                               // HINSTANCE hInstance
                       NULL                                     // LPVOID lpParam
  );

  if (!ghWnd) {
    return FALSE;
  }

  ShowWindow(ghWnd, nCmdShow);
  UpdateWindow(ghWnd);

  return TRUE;
}

void CreateUIControls(HWND hWndParent);
void GetInputFileName(HWND hWndParent);
void Fill_TreeView(const HWND mainHwnd);

#define READ_FROM_BUTTON 1000
#define READ_FROM_EDITBOX 1001
#define FBX_TREEVIEW 1002

// 窗口过程
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  int wmId, wmEvent;

  switch (message) {
    // 创建窗口时，构建UI布局并初始化FBX_SDK
    case WM_CREATE: {
      CreateUIControls(hWnd);
      FbxUtil::Instance().InitializeSdkManagerAndScene();
    } break;

    case WM_COMMAND:
      wmId = LOWORD(wParam);
      wmEvent = HIWORD(wParam);

      // 点击导入按钮时
      switch (wmId) {
        case READ_FROM_BUTTON:
          // 获取选择文件，更新场景树
          GetInputFileName(hWnd);
          Fill_TreeView(hWnd);
          break;

        default:
          return DefWindowProc(hWnd, message, wParam, lParam);
      }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

void CreateUIControls(HWND hWndParent) {
  DWORD dwStyle = WS_CHILD | WS_VISIBLE;

  // 创建读READ_FROM_BUTTON按钮
  CreateWindowEx(0,                        // DWORD dwExStyle,
                 "BUTTON",                 // LPCTSTR lpClassName
                 "Read from ...",          // control caption
                 dwStyle,                  // DWORD dwStyle
                 10,                       // int x
                 10,                       // int y
                 130,                      // int nWidth
                 30,                       // int nHeight
                 hWndParent,               // HWND hWndParent
                 (HMENU)READ_FROM_BUTTON,  // HMENU hMenu or control's ID for WM_COMMAND
                 hInst,                    // HINSTANCE hInstance
                 NULL                      // LPVOID lpParam
  );

  // 创建READ_FROM_EDITBOX编辑框
  CreateWindowEx(WS_EX_STATICEDGE,             // DWORD dwExStyle,
                 "EDIT",                       // LPCTSTR lpClassName
                 " <- select a file to read",  // control caption
                 dwStyle | ES_AUTOHSCROLL,     // DWORD dwStyle
                 150,                          // int x
                 15,                           // int y
                 534,                          // int nWidth
                 20,                           // int nHeight
                 hWndParent,                   // HWND hWndParent
                 (HMENU)READ_FROM_EDITBOX,     // HMENU hMenu or control's ID for WM_COMMAND
                 hInst,                        // HINSTANCE hInstance
                 NULL                          // LPVOID lpParam
  );

  // 创建小字体
  HFONT hf = CreateFont(14,                   // height of font
                        6,                    // average character width
                        0,                    // angle of escapement
                        0,                    // base-line orientation angle
                        40,                   // font weight
                        0,                    // italic attribute option
                        0,                    // underline attribute option
                        0,                    // strikeout attribute option
                        ANSI_CHARSET,         // character set identifier
                        OUT_DEFAULT_PRECIS,   // output precision
                        CLIP_DEFAULT_PRECIS,  // clipping precision
                        0,                    // output quality
                        0,                    // pitch and family
                        "Arial"               // typeface name
  );

  // 设置READ_FROM_EDITBOX的字体
  SendMessage(GetDlgItem(hWndParent, READ_FROM_EDITBOX), WM_SETFONT, (WPARAM)hf, (LPARAM) false);

  // 创建FBX_TREEVIEW控件
  dwStyle = dwStyle | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

  CreateWindowEx(WS_EX_STATICEDGE,     // DWORD dwExStyle,
                 WC_TREEVIEW,          // LPCTSTR lpClassName
                 "",                   // control caption
                 dwStyle,              // DWORD dwStyle
                 10,                   // int x
                 50,                   // int y
                 674,                  // int nWidth
                 686,                  // int nHeight
                 hWndParent,           // HWND hWndParent
                 (HMENU)FBX_TREEVIEW,  // HMENU hMenu or control's ID for WM_COMMAND
                 hInst,                // HINSTANCE hInstance
                 NULL                  // LPVOID lpParam
  );
}

char gszInputFile[_MAX_PATH];  // File name to import

// show the <Open file> dialog
void GetInputFileName(HWND hWndParent) {
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(ofn));

  char szFile[_MAX_PATH];  // buffer for file name
  ZeroMemory(szFile, sizeof(szFile));

  // Initialize OPENFILENAME
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hWndParent;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.lpstrTitle = "Select the file to read ... (use the file type filter)";
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  // get a description of all readers registered in the FBX SDK
  const char* filter = FbxUtil::Instance().GetReaderOFNFilters();
  ofn.lpstrFilter = filter;

  // Display the Open dialog box.
  if (GetOpenFileName(&ofn) == false) {
    // user cancel
    delete filter;
    return;
  }

  delete filter;

  // show the file name selected
  SetWindowText(GetDlgItem(hWndParent, READ_FROM_EDITBOX), szFile);

  // Keep a copy of the file name
  FBXSDK_strcpy(gszInputFile, _MAX_PATH, szFile);
}

void DisplayHierarchy(const HWND hTv);
void DisplayHierarchy_Recurse(const FbxNode* pNode, const HWND hTv, HTREEITEM htiParent);

void Expand_All();
void Expand_All_Recurse(HWND htv, HTREEITEM htvi);

// fill the treeview with the FBX scene content
void Fill_TreeView(const HWND mainHwnd) {
  if (strlen(gszInputFile) == 0)
    return;

  // get the handle of the treeview
  HWND htv = GetDlgItem(mainHwnd, FBX_TREEVIEW);
  if (htv == NULL)
    return;

  // clear the treeview content
  TreeView_DeleteAllItems(htv);

  // show a wait cursor
  HCURSOR oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

  // load the FBX scene
  if (FbxUtil::Instance().LoadFBXScene(gszInputFile) == false) {
    // reset to default cursor
    SetCursor(oldCursor);

    return;
  }

  // display scene hierarchy
  DisplayHierarchy(htv);

  // expand all items of the treeview
  Expand_All();

  // force the root node visible
  TreeView_SelectSetFirstVisible(htv, TreeView_GetRoot(htv));

  // reset to default cursor
  SetCursor(oldCursor);
}

void DisplayHierarchy(const HWND hTv) {
  const FbxNode* rootNode = FbxUtil::Instance().GetRootNode();
  HTREEITEM htvi = InsertTreeViewItem(hTv, rootNode->GetName(), TVI_ROOT);

  for (int i = 0; i < rootNode->GetChildCount(); i++) {
    DisplayHierarchy_Recurse(rootNode->GetChild(i), hTv, htvi);
  }
}

// used to recursively add children nodes
void DisplayHierarchy_Recurse(const FbxNode* pNode, const HWND hTv, HTREEITEM htiParent) {
  // create a new Treeview item with node name and Attribute type name
  HTREEITEM htvi = InsertTreeViewItem(hTv, FbxUtil::GetNodeNameAndAttributeTypeName(pNode).Buffer(), htiParent);

  // show some FbxNode parameters
  Add_TreeViewItem_KFbxNode_Parameters(pNode, hTv, htvi);

  for (int i = 0; i < pNode->GetChildCount(); i++) {
    // recursively call this
    DisplayHierarchy_Recurse(pNode->GetChild(i), hTv, htvi);
  }
}

// used to expand all treeview nodes
void Expand_All() {
  // get the handle of the treeview
  HWND htv = GetDlgItem(ghWnd, FBX_TREEVIEW);
  if (htv == NULL)
    return;

  Expand_All_Recurse(htv, TreeView_GetRoot(htv));

  // force the root node visible on expand
  TreeView_SelectSetFirstVisible(htv, TreeView_GetRoot(htv));
}

// used to expand all treeview items recursively
void Expand_All_Recurse(HWND htv, HTREEITEM htvi) {
  if (htvi == NULL)
    return;

  TreeView_Expand(htv, htvi, TVE_EXPAND);

  while (htvi) {
    // expand all children
    htvi = TreeView_GetChild(htv, htvi);
    Expand_All_Recurse(htv, htvi);

    // expand all siblings
    while (htvi) {
      htvi = TreeView_GetNextSibling(htv, htvi);
      Expand_All_Recurse(htv, htvi);
    }
  }
}

}  // namespace SceneTreeView