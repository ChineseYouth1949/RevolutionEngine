cmake_minimum_required(VERSION 3.24)

enable_testing()
include(GoogleTest)

add_subdirectory(Memory)
add_subdirectory(ECS)
add_subdirectory(ResourceManager)
add_subdirectory(TaskSystem)

set(TEST_TARGETS 
    TestMemory
    TestResourceManager
    TestTaskSystem
)

set(RUN_COMMANDS "")
foreach(target ${TEST_TARGETS})
    list(APPEND RUN_COMMANDS COMMAND $<TARGET_FILE:${target}>)
endforeach()

add_custom_target(RunAllTests
    ${RUN_COMMANDS}
    COMMENT "Run all unit test..."
    USES_TERMINAL
    VERBATIM
)

add_dependencies(RunAllTests ${TEST_TARGETS})