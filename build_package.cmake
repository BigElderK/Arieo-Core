cmake_minimum_required(VERSION 3.20)

# Include the function definition
include($ENV{ARIEO_PACKAGE_BUILDENV_OUTPUT_FOLDER}/cmake/build_engine_project.cmake)

# Check if required environment variables are set
if(NOT DEFINED ENV{ARIEO_PACKAGE_BUILDENV_HOST_PRESET})
    message(FATAL_ERROR "Environment variable ARIEO_PACKAGE_BUILDENV_HOST_PRESET is not defined.")
    exit(1)
endif()

if(NOT DEFINED ENV{ARIEO_PACKAGE_BUILDENV_HOST_BUILD_TYPE})
    message(FATAL_ERROR "Environment variable ARIEO_PACKAGE_BUILDENV_HOST_BUILD_TYPE is not defined.")
    exit(1)
endif()

if(NOT DEFINED ENV{ARIEO_PACKAGE_CORE_BUILD_FOLDER})
    message(FATAL_ERROR "Environment variable ARIEO_PACKAGE_CORE_BUILD_FOLDER is not defined.")
    exit(1)
endif()

if(NOT DEFINED ENV{ARIEO_PACKAGE_CORE_OUTPUT_FOLDER})
    message(FATAL_ERROR "Environment variable ARIEO_PACKAGE_CORE_OUTPUT_FOLDER is not defined.")
    exit(1)
endif()

# Call the function with parameters
build_engine_project(
    SOURCE_CMAKE_LIST_DIR ${CMAKE_CURRENT_LIST_DIR}
    TARGET_PROJECT arieo_core
    PRESET $ENV{ARIEO_PACKAGE_BUILDENV_HOST_PRESET}
    BUILD_TYPE $ENV{ARIEO_PACKAGE_BUILDENV_HOST_BUILD_TYPE}
    BUILD_FOLDER $ENV{ARIEO_PACKAGE_CORE_BUILD_FOLDER}
    OUTPUT_FOLDER $ENV{ARIEO_PACKAGE_CORE_OUTPUT_FOLDER}/$ENV{ARIEO_PACKAGE_BUILDENV_HOST_PRESET}/$ENV{ARIEO_PACKAGE_BUILDENV_HOST_BUILD_TYPE}
)