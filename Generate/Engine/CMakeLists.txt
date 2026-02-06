cmake_minimum_required(VERSION 3.24)

project(REngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CodePath ${CMAKE_SOURCE_DIR}/Engine)

file(GLOB_RECURSE HEAD_FILE
    "${CodePath}/*.h")
file(GLOB_RECURSE SOURCE_FILE
    "${CodePath}/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILE})

# Create namespace alias for modern CMake conventions
add_library(Revolution::Engine ALIAS ${PROJECT_NAME})

# Target config
RE_TARGET_SET(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_compile_definitions(${PROJECT_NAME} PUBLIC UNICODE _UNICODE)
target_compile_definitions(${PROJECT_NAME} PRIVATE RE_EXPORT)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC RE_DEBUG)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC 
    EASTL_EASTDC_VSNPRINTF=0
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /permissive)
endif()

option(USE_EASTL "Enable verbose debug output" ON)
if(USE_EASTL)
    target_compile_definitions(${PROJECT_NAME} PUBLIC RE_USE_EASTL)
endif()

# PCH file
target_precompile_headers(${PROJECT_NAME}
    PUBLIC
    "${CodePath}/Base/PCH.h"
    "${CodePath}/Base/All.h"
)

# include my head file
target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_SOURCE_DIR}
)

# link engine sdk (must before FlatGenerateFun, because it exe find_packge)
include(LinkSDK.cmake)  
include(FlatGener.cmake)

# find fbs file
file(GLOB_RECURSE FBS_FILES
    "${CodePath}/*.fbs"
)

FlatGenerateFun(
    TARGET FLAT_HEADER_FILES
    SCHEMAS ${FBS_FILES}
    INCLUDE_PREFIX 
    OUTPUT_DIR "${CodePath}/FlatBuffers"
    FLAGS --cpp --gen-mutable --gen-object-api
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${FLAT_HEADER_FILES}
)

# link platform lib
target_link_libraries(${PROJECT_NAME} PUBLIC
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
    dxguid.lib
)