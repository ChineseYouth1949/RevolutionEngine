
cmake_minimum_required(VERSION 3.30)

set(TARGET_NAME "TestMemory")

add_executable(${TARGET_NAME} Test.cpp)
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
    "$<TARGET_FILE:REngine>"
    "$<TARGET_FILE_DIR:${TARGET_NAME}>"
    COMMENT "Copying REngine.dll to output directory"
    VERBATIM
)