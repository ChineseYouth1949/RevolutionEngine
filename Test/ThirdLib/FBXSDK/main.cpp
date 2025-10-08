#include "FirstApp.h"
#include "LoadSceneExport.h"
#include "SceneTreeView.h"

#include <assert.h>
#define CheckResult(x) assert(x == 0)

int main() {
  CheckResult(FirstApp::RunExample());
  CheckResult(LoadSceneExport::RunExample());
  CheckResult(SceneTreeView::RunExample());

  return 0;
}