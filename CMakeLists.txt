cmake_minimum_required(VERSION 3.30)

project(Revolution VERSION 1.0)

set(VCPKG_INSTALL_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")
message("VCPKG_INSTALL_PATH : ${VCPKG_INSTALL_PATH}")

include(Generate/LinkSDK.cmake)
include(Generate/LinkQt.cmake)
include(Generate/OutputSet.cmake)

add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(Test)