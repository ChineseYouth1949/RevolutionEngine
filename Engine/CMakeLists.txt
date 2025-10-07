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

# 包含自身头和VCPKG头文件
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${RE_HEADER_DIR}>
    ${VCPKG_INSTALL_PATH}/include
)

set(VCPKG_TARGET_TRIPLET "x64-windows-static-md")

# 链接VPKG库
find_package(rapidjson REQUIRED)
find_package(taskflow REQUIRED)
find_package(directx-headers REQUIRED)
find_package(assimp REQUIRED)
find_package(mimalloc REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE mimalloc)

# External目录
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/")
set(EXTERNAL_HEADER_DIR "${EXTERNAL_DIR}/Include")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXTERNAL_LIB_DIR "${EXTERNAL_DIR}/Debug/Lib")
    set(EXTERNAL_BIN_DIR "${EXTERNAL_DIR}/Debug/Bin/")
else()
    set(EXTERNAL_LIB_DIR "${EXTERNAL_DIR}/Lib")
    set(EXTERNAL_BIN_DIR "${EXTERNAL_DIR}/Bin/")
endif()

# 递归收集所有.lib文件
file(GLOB_RECURSE EXTERNAL_LIBS 
    "${EXTERNAL_LIB_DIR}/*.lib"
)

# 链接到目标
if(EXTERNAL_LIBS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTERNAL_LIBS})
    message(STATUS "Linked external libraries: ${EXTERNAL_LIBS}")
else()
    message(WARNING "No external libraries found in ${EXTERNAL_LIB_DIR}")
endif()

# 链接所需系统lib
target_link_libraries(${PROJECT_NAME} PUBLIC
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
    dxguid.lib
)