project("Editor")

file(GLOB_RECURSE EDITOR_SOURCES 
    "Source/*.cpp"
    "Source/*.h"
)

add_executable(Editor ${EDITOR_SOURCES})

target_include_directories(Editor PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/Source 
)

target_link_libraries(Editor PRIVATE 
    Engine  
)

set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/Build/${RE_PLATFORM}-${RE_ARCH}-${RE_CONFIG}/${PROJECT_NAME})
message(STATUS "${PROJECT_NAME} output dir : ${OUTPUT_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})

if(WIN32 AND TARGET Engine)
    # 使用生成器表达式获取目标文件路径
    add_custom_command(TARGET Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:Engine>"
            "$<TARGET_FILE_DIR:Editor>"
        COMMENT "Deploying Engine DLL to Editor output directory"
    )
endif()

target_compile_definitions(Editor PRIVATE
    RE_EDITOR=1 
)
