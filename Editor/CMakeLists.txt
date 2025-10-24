cmake_minimum_required(VERSION 3.19)

project(REditor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)       # 自动编译 .ui 文件
set(CMAKE_AUTORCC ON)       # 自动编译 .qrc 文件
set(CMAKE_AUTOMOC ON)       # 自动编译包含宏 Q_OBJECT 的文件

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE EDITOR_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

file(GLOB_RECURSE EDITOR_RESOURCES
    "${CMAKE_SOURCE_DIR}/Resource/Editor/*.rc")

# 定义运行目标
add_executable(${PROJECT_NAME} ${EDITOR_SRCS} ${EDITOR_RESOURCES})

# Set app name
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "REditor"
)

# 设置输出路径
SET_TARGET_OUTPUT_DIRS(${PROJECT_NAME})

# 调试模式添加_DEBUG宏
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
endif()

# 链接REngine库
target_link_libraries(${PROJECT_NAME} PUBLIC
    REngine
)

# 拷贝REngine.dll
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:REngine>"  
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Copying REngine.dll (config-aware) to output directory"
    VERBATIM
)

# 链接Qt
set(CMAKE_PREFIX_PATH "${QT_BUILD_ROOT}/lib/cmake/Qt${QT_VERSION_MAJOR}")

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Quick Qml Core Widgets REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

# 拷贝Qt dll
set(QT_DLL_PATH "${QT_BUILD_ROOT}/bin")

LinkQt(${PROJECT_NAME})

# 拷贝Resource
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resource/
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resource/Editor/RuntimeFile/
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resource/
    COMMENT "Copying Resource/ directory to output"
    VERBATIM
)