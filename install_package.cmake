cmake_minimum_required(VERSION 3.20)

# Include the installation function
include($ENV{ARIEO_PACKAGE_BUILDENV_INSTALL_FOLDER}/cmake/install_engine_project.cmake)

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

if(NOT DEFINED ENV{ARIEO_PACKAGE_CORE_INSTALL_FOLDER})
    message(FATAL_ERROR "Environment variable ARIEO_PACKAGE_CORE_INSTALL_FOLDER is not defined.")
    exit(1)
endif()

# Set variables from environment
set(PRESET $ENV{ARIEO_PACKAGE_BUILDENV_HOST_PRESET})
set(BUILD_TYPE $ENV{ARIEO_PACKAGE_BUILDENV_HOST_BUILD_TYPE})
set(BUILD_FOLDER $ENV{ARIEO_PACKAGE_CORE_BUILD_FOLDER})
set(INSTALL_FOLDER $ENV{ARIEO_PACKAGE_CORE_INSTALL_FOLDER})

# Set install prefix for this preset/build type combination
set(INSTALL_PREFIX "${INSTALL_FOLDER}/${PRESET}/${BUILD_TYPE}")

# Clean the install folder before installing
if(EXISTS "${INSTALL_PREFIX}")
    message(STATUS "Cleaning install folder: ${INSTALL_PREFIX}")
    file(REMOVE_RECURSE "${INSTALL_PREFIX}")
endif()

# Create the install directory structure
file(MAKE_DIRECTORY "${INSTALL_PREFIX}")

# Call the installation function
install_engine_project(
    BUILD_FOLDER ${BUILD_FOLDER}
    BUILD_TYPE ${BUILD_TYPE}
    INSTALL_PREFIX ${INSTALL_PREFIX}
)
