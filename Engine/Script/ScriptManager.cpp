#include "ScriptManager.h"
#include "Engine/ECS/Resource/ResourceVisitor.h"
#include "Engine/ECS/World/Transform.h"
#include "Engine/Render/System/ColorVertex.h"
#include "Engine/Render/System/Mesh.h"

namespace re::engine::script {

ScriptManager::~ScriptManager() {
  Release();
}

void ScriptManager::Initialize(ecs::World* world) {
  std::lock_guard lock(m_mutex);
  m_world = world;
  // m_resVisitor = rv;  // copy

  m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::io);

  BindAPIs();
}

void ScriptManager::Release() {
  std::lock_guard lock(m_mutex);
  m_timestamps.clear();
  m_lua = sol::state();
  m_world = nullptr;
  m_resVisitor = ecs::ResourceVisitor();
}

bool ScriptManager::LoadScript(const std::string& path) {
  std::lock_guard lock(m_mutex);
  try {
    sol::load_result lr = m_lua.load_file(path);
    if (!lr.valid()) {
      sol::error err = lr;
      return false;
    }
    sol::protected_function_result r = lr();
    if (!r.valid()) {
      sol::error err = r;
      return false;
    }

    // Capture module return value (could be table or function) for management
    sol::object moduleObj = sol::nil;
    try {
      if (r.return_count() > 0) {
        moduleObj = r.get<sol::object>();
      }
    } catch (...) {
      // moduleObj = sol::nil;
    }

    // store module object for later reload/callback
    m_loadedScripts[path] = moduleObj;

    auto ftime = std::filesystem::last_write_time(path);
    m_timestamps[path] = ftime;

    // call module main if provided
    if (moduleObj.valid() && moduleObj.get_type() == sol::type::table) {
      sol::table tbl = moduleObj;
      sol::object mainObj = tbl["main"];
      if (mainObj.valid() && mainObj.get_type() == sol::type::function) {
        sol::protected_function mf = mainObj;
        mf();
      }
      // call on_reload if exists (first time load -> treat as init)
      sol::object reloadObj = tbl["on_reload"];
      if (reloadObj.valid() && reloadObj.get_type() == sol::type::function) {
        sol::protected_function rf = reloadObj;
        rf();
      }
    }

    return true;
  } catch (...) {
    return false;
  }
}

bool ScriptManager::LoadScriptsFromDirectory(const std::string& dir) {
  std::lock_guard lock(m_mutex);
  try {
    for (auto& e : std::filesystem::directory_iterator(dir)) {
      if (e.path().extension() == ".lua") {
        LoadScript(e.path().string());
      }
    }
    return true;
  } catch (...) {
    return false;
  }
}

bool ScriptManager::ExecuteScript(const std::string& path) {
  std::lock_guard lock(m_mutex);
  try {
    sol::load_result lr = m_lua.load_file(path);
    if (!lr.valid())
      return false;
    sol::protected_function_result r = lr();
    if (!r.valid())
      return false;
    // If script returned a module table, prefer its main
    if (r.return_count() > 0) {
      sol::object moduleObj = r.get<sol::object>();
      if (moduleObj.valid() && moduleObj.get_type() == sol::type::table) {
        sol::table tbl = moduleObj;
        sol::object mainObj = tbl["main"];
        if (mainObj.valid() && mainObj.get_type() == sol::type::function) {
          sol::protected_function mainf = mainObj;
          mainf();
        }
      }
    } else {
      sol::object mainObj = m_lua["main"];
      if (mainObj.valid() && mainObj.get_type() == sol::type::function) {
        sol::protected_function mainf = mainObj;
        mainf();
      }
    }
    return true;
  } catch (...) {
    return false;
  }
}

void ScriptManager::CheckAndReloadChangedScripts() {
  std::lock_guard lock(m_mutex);
  for (auto it = m_timestamps.begin(); it != m_timestamps.end();) {
    try {
      auto cur = std::filesystem::last_write_time(it->first);
      if (cur != it->second) {
        // reload
        std::string path = it->first;
        // call unload if exists
        auto oldIt = m_loadedScripts.find(path);
        if (oldIt != m_loadedScripts.end()) {
          sol::object oldObj = oldIt->second;
          if (oldObj.valid() && oldObj.get_type() == sol::type::table) {
            sol::table ot = oldObj;
            sol::object unload = ot["on_unload"];
            if (unload.valid() && unload.get_type() == sol::type::function) {
              try {
                sol::protected_function uf = unload;
                uf();
              } catch (...) {}
            }
          }
        }

        LoadScript(path);
        it->second = cur;
      }
      ++it;
    } catch (...) {
      ++it;
    }
  }
}

void ScriptManager::BindAPIs() {
  // Bind Resource API
  // if (m_resVisitor.GetResourceManager() != nullptr) {
  //   m_lua["Resource"] = m_lua.create_table();
  //   m_lua["Resource"]["get_by_string"] = [this](const std::string& s) {
  //     auto id = m_resVisitor.GetStringResId(s);
  //     return static_cast<uint64_t>(id.value);
  //   };
  //   m_lua["Resource"]["has"] = [this](uint64_t id) {
  //     return m_resVisitor.HasResource(ecs::ResId(static_cast<uint32_t>(id)));
  //   };
  // }

  // Bind World/Registry APIs
  if (m_world) {
    m_lua["World"] = m_lua.create_table();
    m_lua["World"]["create_entity"] = [this]() {
      return static_cast<uint64_t>(m_world->CreateEntity());
    };
    m_lua["World"]["destroy_entity"] = [this](uint64_t id) {
      m_world->DestroyEntity(static_cast<ecs::Entity>(id));
    };
    m_lua["World"]["has_entity"] = [this](uint64_t id) {
      return m_world->HasEntity(static_cast<ecs::Entity>(id));
    };

    // Add/Remove Transform component
    m_lua["World"]["add_transform"] = [this](uint64_t id, double px, double py, double pz) {
      ecs::Entity e = static_cast<ecs::Entity>(id);
      ecs::Transform t;
      t.position = glm::vec3(static_cast<float>(px), static_cast<float>(py), static_cast<float>(pz));
      m_world->AddComponent<ecs::Transform>(e, t);
      return true;
    };

    m_lua["World"]["remove_transform"] = [this](uint64_t id) {
      ecs::Entity e = static_cast<ecs::Entity>(id);
      return m_world->RemoveComponent<ecs::Transform>(e);
    };

    // Mesh helpers for RenderColorVertex::SysComType (Mesh<ColorVertex,uint32_t>)
    using SysComType = re::engine::render::Mesh<re::engine::render::ColorVertex, uint32_t>;
    m_lua["World"]["add_mesh"] = [this](uint64_t id, sol::table verts, sol::table inds) {
      ecs::Entity e = static_cast<ecs::Entity>(id);
      SysComType com;

      // parse vertices
      for (auto& kv : verts) {
        sol::object v = kv.second;
        if (v.get_type() == sol::type::table) {
          sol::table vt = v;
          float px = vt[1];
          float py = vt[2];
          float pz = vt[3];
          float r = vt[4] ? vt[4] : 1.0f;
          float g = vt[5] ? vt[5] : 1.0f;
          float b = vt[6] ? vt[6] : 1.0f;
          float a = vt[7] ? vt[7] : 1.0f;
          re::engine::render::ColorVertex cv;
          cv.position = glm::vec3(px, py, pz);
          cv.color = glm::vec4(r, g, b, a);
          com.vertexs.push_back(cv);
        }
      }

      // parse indices
      for (auto& kv : inds) {
        sol::object obj = kv.second;
        if (obj.is<int>()) {
          com.indexes.push_back(static_cast<uint32_t>(obj.as<int>()));
        }
      }

      m_world->AddComponent<SysComType>(e, com);
      return true;
    };

    m_lua["World"]["remove_mesh"] = [this](uint64_t id) {
      ecs::Entity e = static_cast<ecs::Entity>(id);
      using SysComType = re::engine::render::Mesh<re::engine::render::ColorVertex, uint32_t>;
      return m_world->RemoveComponent<SysComType>(e);
    };

    // Helpers to access Transform as a lua table
    m_lua["World"]["get_transform"] = [this](uint64_t id) -> sol::object {
      ecs::Entity e = static_cast<ecs::Entity>(id);
      if (!m_world->HasComponents<ecs::Transform>(e))
        return sol::nil;
      auto& t = m_world->GetComponent<ecs::Transform>(e);
      sol::table tbl = m_lua.create_table();
      tbl["px"] = t.position.x;
      tbl["py"] = t.position.y;
      tbl["pz"] = t.position.z;
      tbl["sx"] = t.scale.x;
      tbl["sy"] = t.scale.y;
      tbl["sz"] = t.scale.z;
      return tbl;
    };

    m_lua["World"]["set_transform"] = [this](uint64_t id, sol::table tbl) {
      ecs::Entity e = static_cast<ecs::Entity>(id);
      ecs::Transform t;
      t.position.x = tbl["px"] ? tbl["px"] : 0.0f;
      t.position.y = tbl["py"] ? tbl["py"] : 0.0f;
      t.position.z = tbl["pz"] ? tbl["pz"] : 0.0f;
      t.scale.x = tbl["sx"] ? tbl["sx"] : 1.0f;
      t.scale.y = tbl["sy"] ? tbl["sy"] : 1.0f;
      t.scale.z = tbl["sz"] ? tbl["sz"] : 1.0f;
      m_world->ChangeComponent<ecs::Transform>(e, t);
      return true;
    };

    // Mesh info accessor
    m_lua["World"]["get_mesh_info"] = [this](uint64_t id) -> sol::object {
      using SysComType = re::engine::render::Mesh<re::engine::render::ColorVertex, uint32_t>;
      ecs::Entity e = static_cast<ecs::Entity>(id);
      if (!m_world->HasComponents<SysComType>(e))
        return sol::nil;
      auto& mesh = m_world->GetComponent<SysComType>(e);
      sol::table mt = m_lua.create_table();
      mt["vertex_count"] = static_cast<int>(mesh.vertexs.size());
      mt["index_count"] = static_cast<int>(mesh.indexes.size());
      return mt;
    };
  }
}

}  // namespace re::engine::script
