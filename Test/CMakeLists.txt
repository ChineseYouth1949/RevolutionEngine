cmake_minimum_required(VERSION 3.30)

project(REngineTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_TARGET_TRIPLET "x64-windows-static-md")
find_package(GTest CONFIG REQUIRED)

add_subdirectory(Core)