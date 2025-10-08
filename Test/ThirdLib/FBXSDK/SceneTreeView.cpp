#include "SceneTreeView.h"

#include <commctrl.h>  // for treeview control

#include <fbxsdk.h>

#include <string>
#include <iostream>

namespace SceneTreeView {

ATOM UIRegisterClass(HINSTANCE hInstance);
BOOL InitWindow(HINSTANCE, int);

int RunExample(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
  // 注册窗口类
  UIRegisterClass(hInstance);

  // 初始化窗口并显示
  if (!InitWindow(hInstance, nCmdShow)) {
    return -1;
  }

  // 消息循环
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam;
}

#define MAX_LOADSTRING 100
TCHAR szWindowClass[MAX_LOADSTRING];  // 全局注册窗口类名

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 注册窗口类到szWindowClass
ATOM UIRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
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

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM) {
  return LRESULT();
}

}  // namespace SceneTreeView