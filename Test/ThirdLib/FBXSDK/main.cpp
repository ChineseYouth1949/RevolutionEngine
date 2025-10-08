#include "FirstApp.h"
#include "LoadSceneExport.h"

#include <assert.h>
#define CheckResult(x) assert(x == 0)

int main() {
  CheckResult(FirstApp::RunExample());
  CheckResult(LoadSceneExport::RunExample());

  return 0;
}