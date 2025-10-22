# Qt DLL自动拷贝配置
function(LinkQt target_name)
    if(WIN32 AND QT_DLL_PATH)
        # 定义需要拷贝的Qt核心模块
        set(QT_CORE_MODULES 
            Core
            Gui
            Widgets
        )
        
        # 根据构建类型确定DLL后缀
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(DLL_SUFFIX "d")
        else()
            set(DLL_SUFFIX "")
        endif()
        
        # 为每个模块添加拷贝命令
        foreach(module IN LISTS QT_CORE_MODULES)
            set(dll_name "Qt${QT_VERSION_MAJOR}${module}${DLL_SUFFIX}.dll")
            set(src_dll "${QT_DLL_PATH}/${dll_name}")
            set(dst_dir "$<TARGET_FILE_DIR:${target_name}>")
            
            add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${src_dll}"
                    "${dst_dir}"
                COMMENT "拷贝 ${dll_name} 到输出目录"
                VERBATIM
            )
        endforeach()
        
        # 拷贝平台插件
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory 
                "$<TARGET_FILE_DIR:${target_name}>/platforms"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_DLL_PATH}/../plugins/platforms/qwindows${DLL_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${target_name}>/platforms/"
            COMMENT "拷贝Qt平台插件"
            VERBATIM
        )
    endif()
endfunction()