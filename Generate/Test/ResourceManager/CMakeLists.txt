cmake_minimum_required(VERSION 3.19)

project(TestResourceManager VERSION 1.0)

set(CodePath ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCE_FILE
    "${CodePath}/*.h"
    "${CodePath}/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILE} ${RESOURCE_FILE})

enable_testing()

find_package(GTest CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest)

RE_TARGET_SET(${PROJECT_NAME})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _DEBUG)
endif()

add_dependencies(${PROJECT_NAME} REngine)
target_link_libraries(${PROJECT_NAME} PUBLIC
    REngine
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Resource/"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resource/"
    COMMENT "Copying Runtime Resources"
    VERBATIM
)

set(EDITOR_BIN_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # 拷贝 REngine.dll (通常已经在 bin 下，这里作为二次保险或同步)
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "$<TARGET_FILE:REngine>" "${EDITOR_BIN_DIR}"
        
    # 拷贝整个 D3D12 文件夹（包含 D3D12Core.dll 等）
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_FILE_DIR:REngine>/D3D12" "${EDITOR_BIN_DIR}/D3D12"
        
    COMMENT "Deploying REngine binaries and D3D12 Agility SDK..."
    VERBATIM
)