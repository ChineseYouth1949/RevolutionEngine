#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <assert.h>

#include "RE.h"

int main() {
  std::cout << "Hello RevolutionEngine" << std::endl;

  std::string file = "./Assets/Model/sibenik.gltf";

  bool useAsync = true;
  int count = 1;

  // // async
  // {
  //   int task_system_thread_num = RE::Core::TaskSystem::Instance().GetThreadNum();

  //   unsigned int num_threads = std::thread::hardware_concurrency();
  //   unsigned int importer_num = std::min<int>(num_threads, count);

  //   std::vector<RE::Core::SceneImporter> sceneImporters(importer_num);

  //   auto start = std::chrono::high_resolution_clock::now();

  //   for (size_t i = 0; i < count; i++) {
  //     RE::Core::SceneImporter* import = nullptr;

  //     auto it = sceneImporters.begin();
  //     while (1) {
  //       if (it->IsUseable()) {
  //         break;
  //       }
  //       it++;

  //       if (it == sceneImporters.end()) {
  //         it = sceneImporters.begin();
  //       }
  //     }

  //     assert(it->LoadSceneAsync(file, 0));
  //   }

  //   auto end = std::chrono::high_resolution_clock::now();
  //   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  //   std::cout << "async call use time : " << duration.count() << " milliseconds" << std::endl;

  //   for (auto& importer : sceneImporters) {
  //     importer.WaitAsyncComplete();
  //   }

  //   end = std::chrono::high_resolution_clock::now();
  //   duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  //   std::cout << "async total use time : " << duration.count() << " milliseconds" << std::endl;
  // }

  // // no async
  // {
  //   auto start = std::chrono::high_resolution_clock::now();

  //   RE::Core::SceneImporter sceneImporter;

  //   for (size_t i = 0; i < count; i++) {
  //     assert(sceneImporter.LoadScene(file, 0));
  //   }

  //   auto end = std::chrono::high_resolution_clock::now();
  //   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  //   std::cout << "main use time : " << duration.count() << " milliseconds" << std::endl;
  // }

  return 0;
}