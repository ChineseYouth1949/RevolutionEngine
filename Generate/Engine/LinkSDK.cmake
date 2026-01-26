# only header lib
find_package(directx-headers CONFIG REQUIRED)
find_package(taskflow CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)

find_package(mimalloc CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC mimalloc)

find_package(EASTL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC EASTL)

find_package(flatbuffers CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC flatbuffers::flatbuffers)

# LUAJIT
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUAJIT REQUIRED IMPORTED_TARGET luajit)
target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LUAJIT)

find_package(winpixevent CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Microsoft::WinPixEventRuntime)

find_package(directx12-agility CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Microsoft::DirectX-Headers Microsoft::DirectX-Guids Microsoft::DirectX12-Agility)

if(TARGET Microsoft::DirectX12-Agility)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/D3D12"
        
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:Microsoft::DirectX12-Core>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/D3D12/"
        
        COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> $<$<CONFIG:Debug>:-E> $<$<CONFIG:Debug>:copy_if_different> 
                "$<TARGET_FILE:Microsoft::DirectX12-Layers>" 
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/D3D12/"
        
        COMMENT "Syncing matched D3D12 Agility SDK binaries..."
    )
endif()

if(NOT DX12_Agility_VERSION)
    message(FATAL_ERROR 
        "未找到 DX12_Agility_VERSION 变量！\n"
        "请确保在 CMakePresets.json 的 'cacheVariables' 中定义了该字段，"
        "或在命令行指定 -DDX12_Agility_VERSION=616"
    )
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC 
    RE_D3D12_AGILITY_SDK_VERSION=${DX12_Agility_VERSION}
)
message(STATUS "Agility SDK Version defined from Preset: ${DX12_Agility_VERSION}")

# sdk paths
set(SDK_BIN_PATH "${SDK_ROOT}/bin")
set(SDK_INCLUDE_PATH "${SDK_ROOT}/include")
set(SDK_LIB_PATH "${SDK_ROOT}/lib")

set(VCPKG_INCLUDE_PATH "${VCPKG_INSTALL_PATH}/include")
set(VCPKG_LIB_PATH "${VCPKG_INSTALL_PATH}/lib")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SDK_BIN_PATH "${SDK_ROOT}/debug/bin")
    set(SDK_LIB_PATH "${SDK_ROOT}/debug/lib")
    set(VCPKG_LIB_PATH "${VCPKG_INSTALL_PATH}/debug/lib")
endif()

# 包含VCPKG头、SDK头
target_include_directories(${PROJECT_NAME}
    PUBLIC
    # ${VCPKG_INSTALL_PATH}/include
    # ${VCPKG_INSTALL_PATH}/include/luajit
    ${SDK_INCLUDE_PATH}
)

# 链接FBX SDK
file(GLOB_RECURSE EXTERNAL_LIBS
    "${SDK_LIB_PATH}/alembic-md.lib"
    "${SDK_LIB_PATH}/libfbxsdk-md.lib"
    "${SDK_LIB_PATH}/libxml2-md.lib"
    "${SDK_LIB_PATH}/zlib-md.lib"
)

if(EXTERNAL_LIBS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTERNAL_LIBS})
    message(STATUS "Linked external libraries: ${EXTERNAL_LIBS}")
else()
    message(WARNING "No external libraries found in ${EXTERNAL_LIB_DIR}")
endif()