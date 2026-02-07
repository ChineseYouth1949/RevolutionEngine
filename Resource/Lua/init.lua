-- init.lua
-- 示例脚本：创建实体、访问资源标识、演示 Transform 使用

function main()
  print("Lua init script running")

  if World and World.create_entity then
    local e = World.create_entity()
    print("Created entity:", e)

    -- add a transform using helper
    if World.add_transform then
      World.add_transform(e, 0.0, 0.0, 0.0)
    end

    -- add a simple color mesh
    if World.add_mesh then
      local verts = {
        {0,0,0, 1,0,0,1},
        {1,0,0, 0,1,0,1},
        {0,1,0, 0,0,1,1}
      }
      local inds = {0,1,2}
      World.add_mesh(e, verts, inds)
      local info = World.get_mesh_info(e)
      if info then print("Mesh verts:", info.vertex_count, "inds:", info.index_count) end
    end
  end

  if Resource and Resource.get_by_string then
    local rid = Resource.get_by_string("test_resource")
    print("Resource id for 'test_resource':", rid)
  end
end

-- 导出 main（ScriptManager 执行时会调用 main）

function on_reload()
  print("init.lua reloaded")
end

function on_unload()
  print("init.lua unloading")
end

return { main = main, on_reload = on_reload, on_unload = on_unload }
