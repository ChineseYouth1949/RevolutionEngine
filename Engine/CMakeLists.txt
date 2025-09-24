# RevolutionEngine 项目配置（动态库）
cmake_minimum_required(VERSION 3.19)

project(RevolutionEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 导出头文件目录
set(RE_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include")

file(GLOB_RECURSE LIB_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h")
file(GLOB_RECURSE LIB_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

message(LIB_HDRS ${LIB_HDRS})
message(LIB_SRCS ${LIB_SRCS})

add_library(${PROJECT_NAME} SHARED ${LIB_SRCS} ${LIB_HDRS})

# 设置输出目录
SET_TARGET_OUTPUT_DIRS(${PROJECT_NAME})

# 添加导出宏定义
target_compile_definitions(${PROJECT_NAME} PRIVATE RE_EXPORT)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
endif()

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
    $<BUILD_INTERFACE:${RE_HEADER_DIR}>
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/External/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
    dxguid.lib
)