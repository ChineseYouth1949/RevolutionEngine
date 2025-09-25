#include "RE.h"

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <assert.h>

int main() {
  std::cout << "Hello RevolutionEngine" << std::endl;

  std::string file = "d://CloudMusic//models-nonbsd//FBX//2013_ASCII//pyramob.fbx";

  bool useAsync = true;
  int count = 20;

  // no async
  {
    auto start = std::chrono::high_resolution_clock::now();

    static RE::Core::SceneImporter sceneImporter;

    for (size_t i = 0; i < count; i++) {
      assert(sceneImporter.LoadScene(file, 0));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "耗时: " << duration.count() << " 毫秒" << std::endl;
  }

  // async
  {
    auto start = std::chrono::high_resolution_clock::now();

    unsigned int num_threads = std::thread::hardware_concurrency();

    static std::vector<RE::Core::SceneImporter> sceneImporters(2);

    for (size_t i = 0; i < count; i++) {
      RE::Core::SceneImporter* import = nullptr;

      auto it = sceneImporters.begin();
      while (1) {
        if (it->IsUseable()) {
          break;
        }
        it++;

        if (it == sceneImporters.end()) {
          it = sceneImporters.begin();
        }
      }

      assert(it->LoadScene(file, 0));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "耗时: " << duration.count() << " 毫秒" << std::endl;
  }

  return 0;
}