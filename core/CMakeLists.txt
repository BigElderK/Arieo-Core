cmake_minimum_required (VERSION 3.31)

include(${CMAKE_CURRENT_LIST_DIR}/../../02_engine/cmake/engine_project.cmake)
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    arieo_engine_project(
        arieo_core
        ALIAS arieo::core
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
        LIBS
            log
            android
            arieo::base
            easyloggingpp::easyloggingpp
            yaml-cpp::yaml-cpp
            fmt::fmt
            mathfu::mathfu
            concurrentqueue::concurrentqueue
    )
else()
    arieo_engine_project(
        arieo_core
        ALIAS arieo::core
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
        LIBS
            arieo::base
            easyloggingpp::easyloggingpp
            yaml-cpp::yaml-cpp
            fmt::fmt
            mathfu::mathfu
            concurrentqueue::concurrentqueue
    )
endif()


        