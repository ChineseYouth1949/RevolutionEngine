#include "PassScheduler.h"

namespace re::engine::ecs {
struct PassInfo {
  shared_ptr<Pass> pass;
  tf::Task flowTask;
};

struct AccessInfo {
  unordered_map<PassMutex::AccessType, vector<PassId>> typeToPassIds;
  vector<unique_ptr<tf::Semaphore>> semaphores;
};

struct PassScheduler::Impl {
  tf::Executor executor;
  tf::Taskflow taskflow;
  unordered_map<PassId, PassInfo> infoMap;
  unordered_map<PassMutex::Type, vector<PassId>> mutexTypeToPassIds;

  unordered_map<uint32_t, AccessInfo> accessInfoMap;
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
  d()->mutexTypeToPassIds.clear();
  d()->accessInfoMap.clear();

  d()->infoMap.reserve(paaes.size());

  for (auto pas : paaes) {
    if (pas->GetId().IsValid()) {
      PassInfo info;
      info.pass = pas;
      info.flowTask = d()->taskflow.emplace([pas]() { pas->Execute(); }).name(Convert<std::string>(pas->GetName()));

      d()->infoMap.insert({pas->GetId(), std::move(info)});

      auto& mutexs = pas->GetMutexs();
      for (auto& mut : mutexs) {
        d()->mutexTypeToPassIds[mut.type].push_back(pas->GetId());
        if (mut.type == PassMutex::Type::ResourceAccess || mut.type == PassMutex::Type::ComponentAccess) {
          d()->accessInfoMap[mut.data.accessTag].typeToPassIds[mut.data.accessType].push_back(pas->GetId());
        }
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

  for (auto& [tag, info] : d()->accessInfoMap) {
    auto& readPasses = info.typeToPassIds[PassMutex::AccessType::Read];
    auto& rwPasses = info.typeToPassIds[PassMutex::AccessType::ReadWrite];

    // Multiple readers can access simultaneously, but writer needs exclusive access
    if (!readPasses.empty() && !rwPasses.empty()) {
      // Read-Write scenario: Create semaphore for each reader
      info.semaphores.reserve(readPasses.size() + 1);
      for (size_t i = 0; i < readPasses.size(); ++i) {
        info.semaphores.push_back(GAlloc::make_unique<tf::Semaphore>(1));
        auto& passId = readPasses[i];
        auto& task = d()->infoMap[passId].flowTask;
        task.acquire(*(info.semaphores[i]));
      }
      // Create semaphore for writers (must wait for all readers)
      info.semaphores.push_back(GAlloc::make_unique<tf::Semaphore>(1));
      for (auto& passId : rwPasses) {
        auto& task = d()->infoMap[passId].flowTask;
        for (auto& sem : info.semaphores) {
          task.acquire(*sem);
        }
      }
    } else if (!readPasses.empty()) {
      // Read-only scenario: no sync needed (multiple readers OK)
      // No semaphore required
    } else if (!rwPasses.empty()) {
      // Write-only scenario: single writer exclusivity
      info.semaphores.push_back(GAlloc::make_unique<tf::Semaphore>(1));
      for (auto& passId : rwPasses) {
        auto& task = d()->infoMap[passId].flowTask;
        task.acquire(*(info.semaphores.back()));
      }
    }
  }
}

void PassScheduler::Execute() {
  try {
    d()->executor.run(d()->taskflow).wait();
  } catch (const std::exception& e) {
    RE_ERROR("PassScheduler execution failed: %s", e.what());
    throw;  // Rethrow for caller to handle
  }
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