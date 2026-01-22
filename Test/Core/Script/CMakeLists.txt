
cmake_minimum_required(VERSION 3.30)

set(TARGET_NAME "TestScript")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${TARGET_NAME} Test.cpp)
add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
RE_TARGET_SET(${TARGET_NAME})

enable_testing()

find_package(GTest CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE GTest::gtest)

target_link_libraries(${TARGET_NAME}
    PUBLIC REngine
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Script.lua"
        "$<TARGET_FILE_DIR:${TARGET_NAME}>"
    COMMENT "Copying Script to output directory"
    VERBATIM
)