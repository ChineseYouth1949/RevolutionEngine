#include "Scheduler.h"

namespace re::engine::ecs {
struct StageInfo {
  shared_ptr<Stage> stage;
  tf::Task flowTask;
};

struct Scheduler::Impl {
  tf::Executor executor;
  tf::Taskflow taskflow;
  std::unordered_map<StageId, StageInfo> infoMap;
};

Scheduler::Scheduler() {
  m_pImpl = GAlloc::make_unique<Impl>();
}

Scheduler::~Scheduler() {
  d()->infoMap.clear();
}

void Scheduler::Compile(const vector<shared_ptr<Stage>>& stages) {
  d()->taskflow.clear();
  d()->infoMap.clear();

  d()->infoMap.reserve(stages.size());
  StageInfo info;

  for (auto st : stages) {
    if (st->GetId() != StageIdFactory::Null) {
      info.stage = st;
      info.flowTask = d()->taskflow.emplace([st]() { st->Execute(); }).name(Convert<std::string>(st->GetName()));
      d()->infoMap.insert({st->GetId(), std::move(info)});
    }
  }

  for (auto& st : stages) {
    auto& info = d()->infoMap[st->GetId()];
    for (auto bId : st->GetPredecessors()) {
      if (d()->infoMap.count(bId)) {
        d()->infoMap[bId].flowTask.precede(info.flowTask);
      }
    }
    for (auto aId : st->GetSuccessors()) {
      if (d()->infoMap.count(aId)) {
        info.flowTask.succeed(d()->infoMap[aId].flowTask);
      }
    }
  }
}

void Scheduler::Execute() {
  d()->executor.run(d()->taskflow).wait();
}

void Scheduler::DumpGraph(const string& path) {
  std::filesystem::path fsPath(Convert<std::string>(path));
  if (fsPath.has_parent_path()) {
    std::filesystem::create_directories(fsPath.parent_path());
  }

  std::ofstream ofs(fsPath);

  if (ofs.is_open()) {
    d()->taskflow.dump(ofs);
    ofs.close();
  } else {
    // 这里可以根据你的项目规范抛出异常或记录日志
    // RE_LOG_ERROR(L"Failed to open file for dumping graph: " + path);
  }
}

}  // namespace re::engine::ecs