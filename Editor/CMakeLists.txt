cmake_minimum_required(VERSION 3.19)

project(Editor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 定义运行目标
add_executable(${PROJECT_NAME} main.cpp)

# 设置输出路径
SET_TARGET_OUTPUT_DIRS(${PROJECT_NAME})

# 调试模式添加_DEBUG宏
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
endif()

# WinMain
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)

# 链接RevolutionEngine库
target_link_libraries(${PROJECT_NAME} PRIVATE
    RevolutionEngine
)

set(CMAKE_PREFIX_PATH ${QT_ROOT})

# 链接Qt
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

# 拷贝RevolutionEngine.dll
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:RevolutionEngine>"  
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Copying RevolutionEngine.dll (config-aware) to output directory"
    VERBATIM
)

# 拷贝Assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets/Model
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets/Model
    COMMENT "Copying Assets/Model directory to output"
    VERBATIM
)