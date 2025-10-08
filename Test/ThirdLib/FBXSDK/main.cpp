#include <windows.h>

#include <assert.h>
#define CheckResult(x) assert(x == 0)

#include "FirstApp.h"
#include "LoadSceneExport.h"
#include "SceneTreeView.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
  CheckResult(FirstApp::RunExample(hInstance, hPrevInstance, lpCmdLine, nCmdShow));
  CheckResult(LoadSceneExport::RunExample(hInstance, hPrevInstance, lpCmdLine, nCmdShow));
  CheckResult(SceneTreeView::RunExample(hInstance, hPrevInstance, lpCmdLine, nCmdShow));

  return 0;
}