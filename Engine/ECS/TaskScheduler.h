#pragma once

#include "Engine/Base/All.h"
#include "Task.h"

namespace re::engine::ecs {

class TaskScheduler {
 public:
  TaskScheduler();
  ~TaskScheduler();

  void Compile(const vector<shared_ptr<Task>>& passArray);
  void Execute();
  void DumpGraph(const string& path);

  RE_DECLARE_PRIVATE(TaskScheduler);
};

}  // namespace re::engine::ecs