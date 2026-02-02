#pragma once

#include "Command.h"
#include "Engine/Base/All.h"

namespace re::engine::ecs {

class CommandBuffer {
 public:
  template <typename T, typename... Args>
  void Enqueue(Args&&... args) {
    m_queue.emplace_back(GAlloc::make_unique<T>(std::forward<Args>(args)...));
  }

  RE_FINLINE void EnqueueLua(sol::function func) { m_queue.emplace_back(GAlloc::make_unique<LuaCommand>(std::move(func))); }

  void Flush() {
    for (auto& cmd : m_queue)
      cmd->Execute();
    m_queue.clear();
  }

 private:
  vector<unique_ptr<Command>> m_queue;
};

}  // namespace re::engine::ecs