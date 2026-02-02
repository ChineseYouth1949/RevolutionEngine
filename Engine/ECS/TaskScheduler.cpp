#include "TaskScheduler.h"

namespace re::engine::ecs {
struct TaskInfo {
  shared_ptr<Task> task;
  tf::Task flowTask;
};

struct TaskScheduler::Impl {
  tf::Executor executor;
  tf::Taskflow taskflow;
  std::unordered_map<TaskId, TaskInfo> infoMap;
};

TaskScheduler::TaskScheduler() {
  m_pImpl = GAlloc::make_unique<Impl>();
}

TaskScheduler::~TaskScheduler() {
  d()->infoMap.clear();
}

void TaskScheduler::Compile(const vector<shared_ptr<Task>>& taskArray) {
  d()->taskflow.clear();
  d()->infoMap.clear();

  d()->infoMap.reserve(taskArray.size());
  TaskInfo info;
  for (auto task : taskArray) {
    info.task = task;
    info.flowTask = d()->taskflow.emplace([task]() { task->Execute(); }).name(Convert<std::string>(task->Name()));
    d()->infoMap.insert({task->Id(), std::move(info)});
  }

  for (auto& task : taskArray) {
    auto& info = d()->infoMap[task->Id()];
    for (auto bId : task->Predecessors()) {
      if (d()->infoMap.count(bId)) {
        d()->infoMap[bId].flowTask.precede(info.task);
      }
    }
    for (auto aId : task->Successors()) {
      if (d()->infoMap.count(aId)) {
        info.flowTask.succeed(d()->infoMap[aId].flowTask);
      }
    }
  }
}

void TaskScheduler::Execute() {
  d()->executor.run(d()->taskflow).wait();
}

void TaskScheduler::DumpGraph(const string& path) {
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