cmake_minimum_required(VERSION 3.20)

project("TestRunEngine")

add_executable(${PROJECT_NAME} "sample.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE Engine)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/Engine/Source
)
