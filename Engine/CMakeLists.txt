# RevolutionEngine 项目配置（动态库）
cmake_minimum_required(VERSION 3.19)

project(RevolutionEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 导出头文件目录
set(RE_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include")

# 源代码目录
file(GLOB_RECURSE LIB_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h")
file(GLOB_RECURSE LIB_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

# 定义RevolutionEngine目标
add_library(${PROJECT_NAME} SHARED ${LIB_SRCS} ${LIB_HDRS})

# 设置输出目录和调试版本后缀
SET_TARGET_OUTPUT_DIRS(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_d")

# 添加导出和调试宏定义
target_compile_definitions(${PROJECT_NAME} PRIVATE RE_EXPORT)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
endif()

# 设置VCPKG路径
set(VCPKG_INSTALL_PATH $ENV{VCPKG_ROOT}/installed/x64-windows)
set(VCPKG_INSTALL_STATIC_PATH $ENV{VCPKG_ROOT}/installed/x64-windows-static-md)

# 包含自身头和扩展头文件
target_include_directories(${PROJECT_NAME} PUBLIC
    ${VCPKG_INSTALL_PATH}/include
    ${VCPKG_INSTALL_STATIC_PATH}/include
    $<BUILD_INTERFACE:${RE_HEADER_DIR}>
)

file(GLOB_RECURSE VCPKG_LIB_PATH "${VCPKG_INSTALL_PATH}/lib/*.lib")
file(GLOB_RECURSE VCPKG_STATIC_LIB_PATH "${VCPKG_INSTALL_STATIC_PATH}/lib/*.lib")

# 链接扩展库lib目录
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    file(GLOB_RECURSE VCPKG_LIB_PATH "${VCPKG_INSTALL_PATH}/debug/lib/*.lib")
    file(GLOB_RECURSE VCPKG_STATIC_LIB_PATH "${VCPKG_INSTALL_STATIC_PATH}/debug/lib/*.lib")
endif()

# 链接扩展库lib
target_link_libraries(${PROJECT_NAME} PRIVATE ${VCPKG_LIB_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE ${VCPKG_STATIC_LIB_PATH})

# # 拷贝扩展DLL
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     file(GLOB DLLS "$ENV{VCPKG_ROOT}/installed/x64-windows/debug/bin/*.dll")
# else()
#     file(GLOB DLLS "$ENV{VCPKG_ROOT}/installed/x64-windows/bin/*.dll") 
# endif()

# foreach(DLL ${DLLS})
#     add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy ${DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>
#     )
# endforeach()

# 链接所需系统lib
target_link_libraries(${PROJECT_NAME} PUBLIC
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
    dxguid.lib
)