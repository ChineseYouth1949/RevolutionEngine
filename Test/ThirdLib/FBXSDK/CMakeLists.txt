# TaskSystemTest 项目配置（可执行文件）
cmake_minimum_required(VERSION 3.19)

project(Engine_Test_FBXSDK VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# 定义运行目标
add_executable(${PROJECT_NAME} WIN32 ${HDRS} ${SRCS})

# 设置输出路径
SET_TARGET_OUTPUT_DIRS(${PROJECT_NAME})

# 调试模式添加_DEBUG宏
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
endif()

LINK_ENGINE_SDK(${PROJECT_NAME})

# 拷贝Resource
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resource
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resource/Model
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resource/Model
    COMMENT "Copying Resource/Model directory to output"
    VERBATIM
)