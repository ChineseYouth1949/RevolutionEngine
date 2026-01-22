# add_subdirectory(Memory)
add_subdirectory(STL)
add_subdirectory(Utility)
add_subdirectory(Script)

add_custom_target(TEST_ALL
    COMMAND TestSTL TestUtility TestScript
    COMMENT "Running all tests with detailed output"
)