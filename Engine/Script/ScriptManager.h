#pragma once

#include "Engine/Base/All.h"
#include "Engine/ECS/Resource/ResourceVisitor.h"
#include "Engine/ECS/World/World.h"

#include <sol/sol.hpp>
#include <filesystem>
#include <mutex>

namespace re::engine::script {

class RE_API ScriptManager {
 public:
  ScriptManager() = default;
  ~ScriptManager();

  void Initialize(ecs::World* world);
  void Release();

  bool LoadScript(const std::string& path);
  bool LoadScriptsFromDirectory(const std::string& dir);
  bool ExecuteScript(const std::string& path);

  void CheckAndReloadChangedScripts();

  sol::state& Lua() { return m_lua; }

 private:
  void BindAPIs();

  sol::state m_lua;
  ecs::World* m_world{nullptr};
  ecs::ResourceVisitor m_resVisitor;

  // 管理已加载的脚本模块对象（lua 返回的 table 或函数）
  std::unordered_map<std::string, sol::object> m_loadedScripts;

  std::mutex m_mutex;
  std::unordered_map<std::string, std::filesystem::file_time_type> m_timestamps;
};

}  // namespace re::engine::script
