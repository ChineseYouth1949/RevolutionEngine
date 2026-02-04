#pragma once

#include "Stage.h"

namespace re::engine::ecs {

class Scheduler {
 public:
  Scheduler();
  ~Scheduler();

  void Compile(const vector<shared_ptr<Stage>>& stages);
  void Execute();
  void DumpGraph(const string& path);

  RE_DECLARE_PRIVATE(Scheduler);
};

}  // namespace re::engine::ecs