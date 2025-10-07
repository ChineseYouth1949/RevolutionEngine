function(LINK_ENGINE_SDK target_name)
    # 查找VCPKG中的依赖
    set(VCPKG_TARGET_TRIPLET "x64-windows-static-md")

    find_package(rapidjson REQUIRED)
    find_package(taskflow REQUIRED)
    find_package(directx-headers REQUIRED)
    find_package(assimp REQUIRED)
    find_package(mimalloc REQUIRED)

    # 查找External中的依赖
    set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/External/")
    set(EXTERNAL_HEADER_DIR "${EXTERNAL_DIR}/Include")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(EXTERNAL_LIB_DIR "${EXTERNAL_DIR}/Debug/Lib")
        set(EXTERNAL_BIN_DIR "${EXTERNAL_DIR}/Debug/Bin/")
    else()
        set(EXTERNAL_LIB_DIR "${EXTERNAL_DIR}/Lib")
        set(EXTERNAL_BIN_DIR "${EXTERNAL_DIR}/Bin/")
    endif()

    # 包含VCPKG头、External头
    target_include_directories(${target_name}
        PUBLIC
        ${VCPKG_INSTALL_PATH}/include
        ${EXTERNAL_HEADER_DIR}
    )

    # 链接VCOKG库
    target_link_libraries(${target_name} PRIVATE assimp::assimp)
    target_link_libraries(${target_name} PRIVATE mimalloc)

    # 链接External库
    file(GLOB_RECURSE EXTERNAL_LIBS 
        "${EXTERNAL_LIB_DIR}/*.lib"
    )

    if(EXTERNAL_LIBS)
        target_link_libraries(${target_name} PRIVATE ${EXTERNAL_LIBS})
        message(STATUS "Linked external libraries: ${EXTERNAL_LIBS}")
    else()
        message(WARNING "No external libraries found in ${EXTERNAL_LIB_DIR}")
    endif()
endfunction()