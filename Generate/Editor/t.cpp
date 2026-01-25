cmake_minimum_required(VERSION 3.19) project(REditor VERSION 1.0)

#-- - 1. 全局配置 -- -
    set(CMAKE_CXX_STANDARD 20) set(CMAKE_CXX_STANDARD_REQUIRED ON) set(CMAKE_INCLUDE_CURRENT_DIR ON)

#Qt 自动工具
        set(CMAKE_AUTOUIC ON) set(CMAKE_AUTORCC ON) set(CMAKE_AUTOMOC ON)

#建议：统一所有 Target 的输出目录，这样就不需要手动 copy DLL 了
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} / bin) set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} / bin)

#-- - 2. 查找依赖 -- -
                find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Quick Qml Core Widgets REQUIRED)

#-- - 3. 收集文件 -- -
                    set(CodePath ${CMAKE_SOURCE_DIR} / Editor) file(GLOB_RECURSE SOURCE_FILES
                                                                    "${CodePath}/*.cpp"
                                                                    "${CodePath}/*.h")
                        file(GLOB_RECURSE RESOURCE_FILES "${CMAKE_SOURCE_DIR}/Resource/Editor/*.rc")

#-- - 4. 定义 Target -- -
                            add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${RESOURCE_FILES})

                                target_link_libraries(
                                    ${PROJECT_NAME} PRIVATE REngine Qt$ { QT_VERSION_MAJOR } ::Core Qt$ { QT_VERSION_MAJOR } ::Gui Qt$ {
                                      QT_VERSION_MAJOR
                                    } ::Widgets)

#-- - 5. 资源处理(Runtime Resource) -- -
                                    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} - E copy_directory
                                                       "${CMAKE_SOURCE_DIR}/Resource/Editor/Runtime/"
                                                       "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resource/" COMMENT "Copying Runtime Resources" VERBATIM)

#-- - 6. Qt 标准部署(Windows 专用) -- -
                                        if (WIN32) get_target_property(_qmake_executable Qt$ { QT_VERSION_MAJOR } ::qmake IMPORTED_LOCATION)
                                            get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
                                                find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

                                                    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no -
                                                                       compiler -
                                                                       runtime-- dir
                                                                       "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
                                                                       "$<TARGET_FILE:${PROJECT_NAME}>" #修复：添加目标名称参数 COMMENT
                                                                       "Running windeployqt to copy dependencies..." VERBATIM) endif()
