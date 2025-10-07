#include <assert.h>

#include "FirstApp.h"
#include "LoadAndSave.h"

#define CheckResult(x) assert(x == 0)

int main() {
  CheckResult(FirstApp::RunExample());
  CheckResult(LoadAndSave::RunExample());

  return 0;
}