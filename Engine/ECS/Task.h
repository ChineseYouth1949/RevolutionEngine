#pragma once

#include "Engine/Base/All.h"

#include "DescriptorFun.h"

namespace re::engine::ecs {

class RE_API Pass {
 public:
  Pass(PassId passId = PassIdFactory::Null, const wstring& name = L"") : m_PassId(passId), m_Name(name) {}
  virtual ~Pass() = default;

  PassId GetPassId() const { return m_PassId; }
  const wstring& GetName() const { return m_Name; }

  virtual const vector<PassId>& GetBeforePass() const = 0;
  virtual const vector<PassId>& GetAfterPass() const = 0;

  virtual void Execute() = 0;

 private:
  PassHandle m_PassId;
  wstring m_Name;
  tf::Task m_FlowTask;

  friend class Scheduler;
};

class Scheduler {
 public:
  void Compile(const vector<std::shared_ptr<Pass>>& passArray) {
    m_Taskflow.clear();
    m_PassMap.clear();

    for (auto& pass : passArray) {
      pass->m_FlowTask = m_Taskflow.emplace([pass]() { pass->Execute(); }).name(toStdString(pass->GetName()));
      m_PassMap[pass->GetPassId()] = pass;
    }

    for (auto& pass : passArray) {
      for (auto bId : pass->GetBeforePass()) {
        if (m_PassMap.count(bId))
          pass->m_FlowTask.precede(m_PassMap[bId]->m_FlowTask);
      }
      for (auto aId : pass->GetAfterPass()) {
        if (m_PassMap.count(aId))
          m_PassMap[aId]->m_FlowTask.precede(pass->m_FlowTask);
      }
    }

    BuildPriorityDependencies(passArray);
  }

  void Execute() { m_Executor.run(m_Taskflow).wait(); }

  void DumpGraph(const wstring& path) {
    std::filesystem::path fsPath(toStd(path));
    if (fsPath.has_parent_path()) {
      std::filesystem::create_directories(fsPath.parent_path());
    }

    std::ofstream ofs(fsPath);

    if (ofs.is_open()) {
      m_Taskflow.dump(ofs);
      ofs.close();
    } else {
      // 这里可以根据你的项目规范抛出异常或记录日志
      // RE_LOG_ERROR(L"Failed to open file for dumping graph: " + path);
    }
  }

 private:
  void BuildPriorityDependencies(const vector<std::shared_ptr<Pass>>& passArray) {
    struct TaskInfo {
      tf::Task task;
      bool canParallel;
    };
    // AccessId -> Priority -> Tasks
    std::unordered_map<size_t, std::map<uint32_t, vector<TaskInfo>>> depGroups;

    for (auto& pass : passArray) {
      for (const auto& dep : pass->GetDependencies()) {
        depGroups[dep.id.value][dep.priority].push_back({pass->m_FlowTask, dep.canParallel});
      }
    }

    for (auto& [accessId, priorityMap] : depGroups) {
      tf::Task lastLevelExit;

      for (auto& [priority, tasks] : priorityMap) {
        vector<tf::Task> currentParallelTasks;
        vector<tf::Task> currentSerialTasks;

        for (const auto& info : tasks) {
          if (info.canParallel)
            currentParallelTasks.push_back(info.task);
          else
            currentSerialTasks.push_back(info.task);
        }

        if (!lastLevelExit.empty()) {
          for (auto& info : tasks) {
            lastLevelExit.precede(info.task);
          }
        }

        tf::Task currentLayerMidSync;
        if (!currentSerialTasks.empty() && currentParallelTasks.size() > 1) {
          std::string syncName = std::to_string(accessId) + "_" + std::to_string(priority) + "_mid_sync";
          currentLayerMidSync = m_Taskflow.placeholder().name(syncName);

          for (auto& pTask : currentParallelTasks) {
            pTask.precede(currentLayerMidSync);
          }
          currentLayerMidSync.precede(currentSerialTasks.front());
        } else if (!currentSerialTasks.empty() && currentParallelTasks.size() == 1) {
          currentParallelTasks[0].precede(currentSerialTasks.front());
        }

        if (!currentSerialTasks.empty()) {
          for (size_t i = 0; i + 1 < currentSerialTasks.size(); ++i) {
            currentSerialTasks[i].precede(currentSerialTasks[i + 1]);
          }
        }

        if (!currentSerialTasks.empty()) {
          lastLevelExit = currentSerialTasks.back();
        } else if (currentParallelTasks.size() > 1) {
          std::string syncName = std::to_string(accessId) + "_" + std::to_string(priority) + "_end_sync";
          lastLevelExit = m_Taskflow.placeholder().name(syncName);
          for (auto& pTask : currentParallelTasks) {
            pTask.precede(lastLevelExit);
          }
        } else if (currentParallelTasks.size() == 1) {
          lastLevelExit = currentParallelTasks[0];
        }
      }
    }
  }

  tf::Executor m_Executor;
  tf::Taskflow m_Taskflow;
  std::unordered_map<PassId, std::shared_ptr<Pass>> m_PassMap;
};
}  // namespace re::engine::ecs