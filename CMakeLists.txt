cmake_minimum_required (VERSION 3.31)

include($ENV{ARIEO_PACKAGE_BUILDENV_INSTALL_FOLDER}/cmake/build_environment.cmake)
project(ArieoEngine)
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    arieo_engine_project(
        arieo_core
        PROJECT_TYPE static_library
        PUBLIC_INCLUDE_FOLDERS
            ${CMAKE_CURRENT_LIST_DIR}/public/include
        SOURCES
            ${CMAKE_CURRENT_LIST_DIR}/private/src/*/*/*.cpp
            ${CMAKE_CURRENT_LIST_DIR}/private/src/*/*.cpp
            ${CMAKE_CURRENT_LIST_DIR}/private/src/*.cpp
        PACKAGES
            easyloggingpp
            yaml-cpp
            fmt
            mathfu
            concurrentqueue
        PUBLIC_LIBS
            yaml-cpp::yaml-cpp
            fmt::fmt
            mathfu::mathfu
            concurrentqueue::concurrentqueue
        PRIVATE_LIBS
            log
            android
            easyloggingpp::easyloggingpp
            
    )
else()
    arieo_engine_project(
        arieo_core
        PROJECT_TYPE static_library
        PUBLIC_INCLUDE_FOLDERS
            ${CMAKE_CURRENT_LIST_DIR}/public/include
        SOURCES
            ${CMAKE_CURRENT_LIST_DIR}/private/src/*/*/*.cpp
            ${CMAKE_CURRENT_LIST_DIR}/private/src/*/*.cpp
            ${CMAKE_CURRENT_LIST_DIR}/private/src/*.cpp
        PACKAGES
            easyloggingpp
            yaml-cpp
            fmt
            mathfu
            concurrentqueue
        PUBLIC_LIBS
            yaml-cpp::yaml-cpp
            fmt::fmt
            mathfu::mathfu
            concurrentqueue::concurrentqueue
        PRIVATE_LIBS
            easyloggingpp::easyloggingpp
            
    )
endif()
        