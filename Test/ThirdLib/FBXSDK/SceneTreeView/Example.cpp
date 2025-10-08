#include "Example.h"
#include "UI.h"

namespace SceneTreeView {

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

}  // namespace SceneTreeView