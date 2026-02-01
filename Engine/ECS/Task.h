#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
using PassId = uint32_t;

class RE_API Pass {
 public:
  Pass(PassId id, const string& name = "") : m_id(id), m_Name(name) {}
  virtual ~Pass() = default;

  PassId Id() const { return m_id; }
  const string& Name() const { return m_Name; }

  const vector<PassId>& BeforePasses() const { return m_BeforPassIds; }
  const vector<PassId>& AfterPasses() const { return m_AfterPassIds; }

  virtual void Execute() = 0;

 protected:
  PassId m_id;
  string m_Name;

  vector<PassId> m_BeforPassIds;
  vector<PassId> m_AfterPassIds;
};

class Scheduler {
  struct PassInfo {
    shared_ptr<Pass> pass;
    tf::Task task;
  };

 public:
  void Compile(const vector<shared_ptr<Pass>>& passArray) {
    m_Taskflow.clear();
    m_PassInfoMap.clear();

    m_PassInfoMap.reserve(passArray.size());
    PassInfo info;
    for (auto pass : passArray) {
      info.pass = pass;
      info.task = m_Taskflow.emplace([pass]() { pass->Execute(); }).name(Convert<std::string>(pass->Name()));
      m_PassInfoMap.insert({pass->Id(), std::move(info)});
    }

    for (auto& pass : passArray) {
      auto& passInfo = m_PassInfoMap[pass->Id()];
      for (auto bId : pass->BeforePasses()) {
        if (m_PassInfoMap.count(bId)) {
          m_PassInfoMap[bId].task.precede(passInfo.task);
        }
      }
      for (auto aId : pass->AfterPasses()) {
        if (m_PassInfoMap.count(aId)) {
          passInfo.task.succeed(m_PassInfoMap[aId].task);
        }
      }
    }
  }

  void Execute() { m_Executor.run(m_Taskflow).wait(); }

  void DumpGraph(const string& path) {
    std::filesystem::path fsPath(Convert<std::string>(path));
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
  tf::Executor m_Executor;
  tf::Taskflow m_Taskflow;
  std::unordered_map<PassId, PassInfo> m_PassInfoMap;
};

}  // namespace re::engine::ecs