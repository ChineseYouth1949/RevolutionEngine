cmake_minimum_required(VERSION 3.24)

include(Tools/PreConfig.cmake)
include(Tools/CheckSDK.cmake)
include(Tools/LinkEngine.cmake)

add_subdirectory(Engine)
add_subdirectory(Editor)
# add_subdirectory(Test)