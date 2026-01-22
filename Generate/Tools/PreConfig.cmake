set(VCPKG_INSTALL_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")

if(MSVC)
    add_compile_options(/EHsc)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

function(RE_TARGET_SET target)
    set(OUTPUT_BASE_DIR "${RUNTIME_ARCHIVE_LIBRARY_BASE_DIRECTORY}")
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_BASE_DIR}/${PROJECT_NAME}/Bin"
        ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_BASE_DIR}/${PROJECT_NAME}/Lib"
        LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_BASE_DIR}/${PROJECT_NAME}/Lib"
    )
endfunction()