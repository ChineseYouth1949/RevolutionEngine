#include "PassScheduler.h"

namespace re::engine::ecs {
struct PassInfo {
  shared_ptr<Pass> pass;
  tf::Task flowTask;
};

struct PassMutexinfo {
  vector<PassId> passsIds;
  unique_ptr<tf::Semaphore> sem;
};

struct PassScheduler::Impl {
  tf::Executor executor;
  tf::Taskflow taskflow;
  unordered_map<PassId, PassInfo> infoMap;
  unordered_map<PassMutexId, PassMutexinfo> mutexInfoMap;
};

PassScheduler::PassScheduler() {
  m_pImpl = GAlloc::make_unique<Impl>();
}

PassScheduler::~PassScheduler() {
  d()->executor.wait_for_all();
}

void PassScheduler::Compile(const vector<shared_ptr<Pass>>& paaes) {
  d()->taskflow.clear();
  d()->infoMap.clear();
  d()->mutexInfoMap.clear();

  d()->infoMap.reserve(paaes.size());

  for (auto pas : paaes) {
    if (pas->GetId() != PassIdFactory::Null) {
      PassInfo info;
      info.pass = pas;
      info.flowTask = d()->taskflow.emplace([pas]() { pas->Execute(); }).name(Convert<std::string>(pas->GetName()));

      d()->infoMap.insert({pas->GetId(), std::move(info)});

      auto& mutexIds = pas->GetMutexIds();
      for (auto mi : mutexIds) {
        d()->mutexInfoMap[mi].passsIds.push_back(pas->GetId());
      }
    }
  }

  for (auto& pas : paaes) {
    auto& info = d()->infoMap[pas->GetId()];
    for (auto bId : pas->GetPredecessors()) {
      if (d()->infoMap.count(bId)) {
        d()->infoMap[bId].flowTask.precede(info.flowTask);
      }
    }
  }

  for (auto& [mutexId, mutexInfo] : d()->mutexInfoMap) {
    mutexInfo.sem = GAlloc::make_unique<tf::Semaphore>(1);

    for (auto passId : mutexInfo.passsIds) {
      auto& task = d()->infoMap[passId].flowTask;
      task.acquire(*(mutexInfo.sem), 1).release(*(mutexInfo.sem), 1);
    }
  }
}

void PassScheduler::Execute() {
  d()->executor.run(d()->taskflow).wait();
}

void PassScheduler::DumpGraph(const string& path) {
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