if(WIN32 AND QT_DLL_PATH)
    set(QT_CORE_MODULES 
        Core
        Gui
        Widgets
    )
        
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(DLL_SUFFIX "d")
    else()
        set(DLL_SUFFIX "")
    endif()
        
    foreach(module IN LISTS QT_CORE_MODULES)
        set(dll_name "Qt${QT_VERSION_MAJOR}${module}${DLL_SUFFIX}.dll")
        set(src_dll "${QT_DLL_PATH}/${dll_name}")
        set(dst_dir "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
            
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${src_dll}"
                "${dst_dir}"
            COMMENT "copy ${dll_name} to bin dir"
            VERBATIM
        )
    endforeach()
        
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT_DLL_PATH}/../plugins/platforms/qwindows${DLL_SUFFIX}.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms/"
        COMMENT "copy qt plugins"
        VERBATIM
    )
endif()