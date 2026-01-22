
message(STATUS "====Check SDK Begin====")

# vcpkg info
message(STATUS "VCPKG_ROOT: ${VCPKG_ROOT}")
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
message(STATUS "VCPKG_INSTALL_PATH: ${VCPKG_INSTALL_PATH}")

# qt info
message(STATUS "QT_ROOT: ${QT_ROOT}")
message(STATUS "QT_VERSION_MAJOR: ${QT_VERSION_MAJOR}")

# sdk info
message(STATUS "SDK_ROOT: ${SDK_ROOT}")

# check valid
function(CheckPathExit pathName)
    if(DEFINED ${pathName})
        if(EXISTS "${${pathName}}")
        else()
            message(FATAL_ERROR "Path not found, ${pathName} : ${${pathName}}")
        endif()
    else()
        message(FATAL_ERROR "Path variable not defined : ${pathName}")
    endif()
endfunction()

function(CheckFileExist fileName)
    if(DEFINED ${fileName})
        if(EXISTS "${${fileName}}" AND NOT IS_DIRECTORY "${${fileName}}")
        else()
            message(FATAL_ERROR "File not found, ${fileName} : ${${fileName}}")
        endif()
    else()
        message(FATAL_ERROR "File variable not defined : ${fileName}")
    endif()
endfunction()

CheckPathExit(VCPKG_ROOT)
CheckPathExit(VCPKG_INSTALL_PATH)
CheckFileExist(CMAKE_TOOLCHAIN_FILE)
CheckPathExit(QT_ROOT)
CheckPathExit(SDK_ROOT)

message(STATUS "====Check SDK Valid End====")