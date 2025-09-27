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

# 设置扩展库目录，链接扩展库和自身头文件（外部可使用）
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External")
target_include_directories(${PROJECT_NAME} 
    PUBLIC 
    $<BUILD_INTERFACE:${RE_HEADER_DIR}>
    ${EXTERNAL_DIR}/include
)

# 设置扩展库dll和lib目录
set(EXTERNAL_BIN_DIR "${EXTERNAL_DIR}/bin")
set(EXTERNAL_LIB_DIR "${EXTERNAL_DIR}/lib")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(EXTERNAL_BIN_DIR "${EXTERNAL_DIR}/debug/bin")
    set(EXTERNAL_LIB_DIR "${EXTERNAL_DIR}/debug/lib")
endif()

# 链接扩展库lib
file(GLOB_RECURSE EXTERNAL_LIB_FILES "${EXTERNAL_LIB_DIR}/*.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTERNAL_LIB_FILES})

# 拷贝扩展库dll
file(GLOB_RECURSE EXTERNAL_BIN_FILES "${EXTERNAL_BIN_DIR}/*.dll")
foreach(DLL_FILE ${EXTERNAL_BIN_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DLL_FILE}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach()

# 链接所需系统dll
target_link_libraries(${PROJECT_NAME} PUBLIC
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
    dxguid.lib
)