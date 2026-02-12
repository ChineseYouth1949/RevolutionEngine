cmake_minimum_required(VERSION 3.24)

project(REditor VERSION 1.0)

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

# Link against namespaced engine target
RE_TARGET_SET(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "REditor"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
endif()


# Add Qt paths from QT_ROOT if defined
if(DEFINED QT_ROOT)
    list(APPEND CMAKE_PREFIX_PATH "${QT_ROOT}")
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Quick Qml Core Widgets REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Revolution::Engine
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
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

LinkEngine(${PROJECT_NAME})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SDK_BIN_PATH "${SDK_ROOT}/debug/bin")
    set(SDK_LIB_PATH "${SDK_ROOT}/debug/lib")
    set(VCPKG_LIB_PATH "${VCPKG_INSTALL_PATH}/debug/lib")
endif()

# 包含VCPKG头、SDK头
target_include_directories(${PROJECT_NAME}
    PUBLIC
    # ${VCPKG_INSTALL_PATH}/include
    # ${VCPKG_INSTALL_PATH}/include/luajit
    ${SDK_INCLUDE_PATH}
)

file(GLOB_RECURSE EXTERNAL_LIBS "${SDK_LIB_PATH}/*.lib")

if(EXTERNAL_LIBS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTERNAL_LIBS})
    message(STATUS "Linked external libraries: ${EXTERNAL_LIBS}")
else()
    message(WARNING "No external libraries found in ${EXTERNAL_LIB_DIR}")
endif()