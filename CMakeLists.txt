cmake_minimum_required(VERSION 3.30)

project(Revolution VERSION 1.0)

set(VCPKG_INSTALL_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")
message("VCPKG_INSTALL_PATH : ${VCPKG_INSTALL_PATH}")

function(SET_TARGET_OUTPUT_DIRS target_name)
    set(OUTPUT_BASE_DIR "${RUNTIME_ARCHIVE_LIBRARY_BASE_DIRECTORY}")
    set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_BASE_DIR}/${PROJECT_NAME}/Bin"
            ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_BASE_DIR}/${PROJECT_NAME}/Lib"
            LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_BASE_DIR}/${PROJECT_NAME}/Lib"
    )
endfunction()

add_subdirectory(Engine)
add_subdirectory(Editor)