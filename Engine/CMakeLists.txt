# RevolutionEngine 项目配置（动态库）
cmake_minimum_required(VERSION 3.30)

project(RevolutionEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 导出头文件目录
set(RE_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include")

# 源代码目录
file(GLOB_RECURSE LIB_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h")
file(GLOB_RECURSE LIB_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

# 定义目标
add_library(${PROJECT_NAME} SHARED ${LIB_SRCS} ${LIB_HDRS})

# 设置输出目录和调试版本后缀
SET_TARGET_OUTPUT_DIRS(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_d")

# 添加导出和调试宏定义
target_compile_definitions(${PROJECT_NAME} PRIVATE RE_EXPORT)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
endif()

# 包含自身头和扩展头文件
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${RE_HEADER_DIR}>
    ${VCPKG_INSTALL_PATH}/include
)

# 链接扩展库
find_package(assimp REQUIRED static-md)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

find_package(mimalloc REQUIRED static-md)
target_link_libraries(${PROJECT_NAME} PRIVATE mimalloc-static)

# 链接所需系统lib
target_link_libraries(${PROJECT_NAME} PUBLIC
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
    dxguid.lib
)