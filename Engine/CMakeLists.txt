cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE ENGINE_SOURCES 
    "Source/*.cpp"
    "Source/*.h"
)

add_library(Engine SHARED ${ENGINE_SOURCES})

target_compile_definitions(Engine PRIVATE
    RE_BUILD_DLL
)