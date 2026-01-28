cmake_minimum_required(VERSION 3.19)

project(REditor VERSION 1.0)

list(APPEND CMAKE_PREFIX_PATH "D:/Software/Qt/6.9.3/msvc2022_64")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)   

set(CodePath ${CMAKE_SOURCE_DIR}/Editor)

file(GLOB_RECURSE SOURCE_FILE
    "${CodePath}/*.h"
    "${CodePath}/*.cpp"
    "${CodePath}/*.ui")
file(GLOB_RECURSE RESOURCE_FILE
    "${CMAKE_SOURCE_DIR}/Resource/Editor/*.rc")

add_executable(${PROJECT_NAME} ${SOURCE_FILE} ${RESOURCE_FILE})

RE_TARGET_SET(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "REditor"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
endif()

add_dependencies(${PROJECT_NAME} REngine)
target_link_libraries(${PROJECT_NAME} PUBLIC
    REngine
)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Quick Qml Core Widgets REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    REngine
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/Resource/"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resource/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Resource/"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resource/"
    COMMENT "Copying Runtime Resources"
    VERBATIM
)

if(WIN32)
    get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --no-compiler-runtime
                --dir "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
                "$<TARGET_FILE:${PROJECT_NAME}>"  # 修复：添加目标名称参数
        COMMENT "Running windeployqt to copy dependencies..."
        VERBATIM
    )
endif()

set(EDITOR_BIN_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         "$<TARGET_FILE_DIR:REngine>" "${EDITOR_BIN_DIR}"
        
#     COMMENT "Deploying REngine binaries and D3D12 Agility SDK..."
#     VERBATIM
# )

add_custom_target(DeployREngine
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_FILE_DIR:REngine>" "${EDITOR_BIN_DIR}"
    COMMENT "Deploying REngine binaries and D3D12 Agility SDK..."
    VERBATIM
)

add_dependencies(${PROJECT_NAME} DeployREngine)
add_dependencies(DeployREngine REngine)