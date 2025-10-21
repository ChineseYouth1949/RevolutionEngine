function(LINK_ENGINE_SDK target_name)
    # 查找VCPKG中的依赖
    set(VCPKG_TARGET_TRIPLET "x64-windows-static-md")

    find_package(rapidjson REQUIRED)
    find_package(taskflow REQUIRED)
    find_package(directx-headers REQUIRED)
    find_package(assimp REQUIRED)
    find_package(mimalloc REQUIRED)

    # 查询FBX SDK
    set(FBX_SDK_ROOT "${RE_SDK_ROOT}/FBXSDK")
    set(FBX_SDK_INCLUDE_PATH "${FBX_SDK_ROOT}/include")
    set(FBX_SDK_LIB_PATH "${FBX_SDK_ROOT}/lib/x64/release")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(FBX_SDK_LIB_PATH "${FBX_SDK_ROOT}/lib/x64/debug")
    endif()

    # 包含VCPKG头、SDK头
    target_include_directories(${target_name}
        PUBLIC
        ${VCPKG_INSTALL_PATH}/include
        ${FBX_SDK_INCLUDE_PATH}
    )

    # 链接VCOKG库
    target_link_libraries(${target_name} PRIVATE assimp::assimp)
    target_link_libraries(${target_name} PRIVATE mimalloc)

    # 链接SDK库
    file(GLOB_RECURSE EXTERNAL_LIBS 
        #FBX SDK
        "${FBX_SDK_LIB_PATH}/alembic-md.lib"
        "${FBX_SDK_LIB_PATH}/libfbxsdk-md.lib"
        "${FBX_SDK_LIB_PATH}/libxml2-md.lib"
        "${FBX_SDK_LIB_PATH}/zlib-md.lib"
    )

    if(EXTERNAL_LIBS)
        target_link_libraries(${target_name} PRIVATE ${EXTERNAL_LIBS})
        message(STATUS "Linked external libraries: ${EXTERNAL_LIBS}")
    else()
        message(WARNING "No external libraries found in ${EXTERNAL_LIB_DIR}")
    endif()
endfunction()