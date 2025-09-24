# RETest-LinkEngine 项目配置（可执行文件）
cmake_minimum_required(VERSION 3.19)

project(RETest-LinkEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp)

SET_TARGET_OUTPUT_DIRS(${PROJECT_NAME})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
endif()

# 链接RevolutionEngine库
target_link_libraries(${PROJECT_NAME} PUBLIC
    RevolutionEngine
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
    dxguid.lib
)
