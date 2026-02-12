#pragma once

#include "Pass.h"

namespace re::engine::ecs {

class RE_API PassScheduler {
 public:
  PassScheduler();
  ~PassScheduler();

  void Compile(const vector<shared_ptr<Pass>>& passes);
  void Execute();
  void DumpGraph(const string& path);

  RE_DECLARE_PRIVATE
};

}  // namespace re::engine::ecs