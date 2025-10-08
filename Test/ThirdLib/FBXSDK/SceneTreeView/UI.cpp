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

// 窗口过程所需函数
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

      switch (wmId) {
        case READ_FROM_BUTTON:
          // 点击导入按钮时：获取选择文件，更新场景树
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

// 创建UI布局
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

  // 设置READ_FROM_EDITBOX的字体为小字体hf
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

// 显示<打开文件>对话框
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

  // 获取FBX SDK中注册的所有读取器的描述
  const char* filter = FbxUtil::Instance().GetReaderOFNFilters();
  ofn.lpstrFilter = filter;

  //// 显示打开对话框。
  if (GetOpenFileName(&ofn) == false) {
    // 用户取消
    delete filter;
    return;
  }

  delete filter;

  // 显示所选的文件名
  SetWindowText(GetDlgItem(hWndParent, READ_FROM_EDITBOX), szFile);

  // 保留文件名的副本
  FBXSDK_strcpy(gszInputFile, _MAX_PATH, szFile);
}

// 更新场景图所需函数
void DisplayHierarchy(const HWND hTv);
void DisplayHierarchy_Recurse(const FbxNode* pNode, const HWND hTv, HTREEITEM htiParent);
void Expand_All();
void Expand_All_Recurse(HWND htv, HTREEITEM htvi);

// 使用FBX场景内容填充树视图
void Fill_TreeView(const HWND mainHwnd) {
  // 检查导入文件路径不为空
  if (strlen(gszInputFile) == 0)
    return;

  // 获取树视图的句柄
  HWND htv = GetDlgItem(mainHwnd, FBX_TREEVIEW);
  if (htv == NULL)
    return;

  // 清除树视图内容
  TreeView_DeleteAllItems(htv);

  // 显示等待光标
  HCURSOR oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

  // 加载 FBX 场景
  if (FbxUtil::Instance().LoadFBXScene(gszInputFile) == false) {
    // 重置为默认光标
    SetCursor(oldCursor);

    return;
  }

  // 显示场景层次
  DisplayHierarchy(htv);

  // 展开树视图的所有项目
  Expand_All();

  // 强制根节点可见
  TreeView_SelectSetFirstVisible(htv, TreeView_GetRoot(htv));

  // 重置为默认光标
  SetCursor(oldCursor);
}

// 显示场景层次
void DisplayHierarchy(const HWND hTv) {
  // 获取FBX场景根节点，插入根节点名称
  const FbxNode* rootNode = FbxUtil::Instance().GetRootNode();
  HTREEITEM htvi = InsertTreeViewItem(hTv, rootNode->GetName(), TVI_ROOT);

  // 递归显示
  for (int i = 0; i < rootNode->GetChildCount(); i++) {
    DisplayHierarchy_Recurse(rootNode->GetChild(i), hTv, htvi);
  }
}

// 递归添显示子节点
void DisplayHierarchy_Recurse(const FbxNode* pNode, const HWND hTv, HTREEITEM htiParent) {
  // 使用节点名称和属性类型名称创建一个新的Treeview项
  HTREEITEM htvi = InsertTreeViewItem(hTv, FbxUtil::GetNodeNameAndAttributeTypeName(pNode).Buffer(), htiParent);

  // 显示一些FbxNode参数（如Translation和Visibility）
  Add_TreeViewItem_KFbxNode_Parameters(pNode, hTv, htvi);

  for (int i = 0; i < pNode->GetChildCount(); i++) {
    // 递归调用
    DisplayHierarchy_Recurse(pNode->GetChild(i), hTv, htvi);
  }
}

// 用于展开所有树视图节点
void Expand_All() {
  // 获取树视图的句柄
  HWND htv = GetDlgItem(ghWnd, FBX_TREEVIEW);
  if (htv == NULL)
    return;

  // 递归展开
  Expand_All_Recurse(htv, TreeView_GetRoot(htv));

  // 强制根节点在展开时可见
  TreeView_SelectSetFirstVisible(htv, TreeView_GetRoot(htv));
}

// 递归展开
void Expand_All_Recurse(HWND htv, HTREEITEM htvi) {
  if (htvi == NULL)
    return;

  TreeView_Expand(htv, htvi, TVE_EXPAND);

  while (htvi) {
    // 展开所有子项
    htvi = TreeView_GetChild(htv, htvi);
    Expand_All_Recurse(htv, htvi);

    // 展开所有兄弟
    while (htvi) {
      htvi = TreeView_GetNextSibling(htv, htvi);
      Expand_All_Recurse(htv, htvi);
    }
  }
}

}  // namespace SceneTreeView