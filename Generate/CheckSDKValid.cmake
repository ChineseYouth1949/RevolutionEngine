
function(CheckSDKValid)
    message(STATUS "====Check SDK Valid Begin====")
    
    # vcpkg info
    message(STATUS "VCPKG_ROOT: ${VCPKG_ROOT}")
    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
    message(STATUS "VCPKG_INSTALL_PATH : ${VCPKG_INSTALL_PATH}")

    # qt info
    message(STATUS "QT_BUILD_ROOT: ${QT_BUILD_ROOT}")
    message(STATUS "QT_VERSION_MAJOR: ${QT_VERSION_MAJOR}")

    # other special SDKs info
    message(STATUS "RE_SDK_ROOT: ${RE_SDK_ROOT}")
    
    # check valid
    if(EXISTS "${VCPKG_ROOT}")
    else()
        message(WARNING " Path not found: ${VCPKG_ROOT}")
    endif()

    if(EXISTS "${QT_BUILD_ROOT}")
    else()
        message(WARNING " Path not found: ${QT_BUILD_ROOT}")
    endif()
    
    if(EXISTS "${RE_SDK_ROOT}")
    else()
        message(WARNING " Path not found: ${RE_SDK_ROOT}")
    endif()
    
    if(EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    else()
        message(WARNING " File not found: ${CMAKE_TOOLCHAIN_FILE}")
    endif()
    
    message(STATUS "====Check SDK Valid End====")
endfunction()
