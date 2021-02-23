# Copyright: Universidad Carlos III de Madrid (C) 2017;
# Authors: Raul de Santos Rico & Juan Gonzalez Victores
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(ROBOTICSLAB_SPEECH LANGUAGES CXX)

# Add suffix for debug libraries.
if(MSVC)
    message(STATUS "Running on windows")
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Let the user specify a configuration (only single-config generators).
if(NOT CMAKE_CONFIGURATION_TYPES)
    # Possible values.
    set(_configurations Debug Release MinSizeRel RelWithDebInfo)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${_configurations})

    foreach(_conf ${_configurations})
        set(_conf_string "${_conf_string} ${_conf}")
    endforeach()

    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING
                 "Choose the type of build, options are:${_conf_string}")

    if(NOT CMAKE_BUILD_TYPE)
        # Encourage the user to specify build type.
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE Release)
    endif()
endif()

# Pick up our cmake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/find-modules)

# Hard dependencies.
find_package(YCM 0.11 REQUIRED)
find_package(YARP 3.3 REQUIRED COMPONENTS os dev
                               OPTIONAL_COMPONENTS idl_tools)

# Soft dependencies.
find_package(Doxygen QUIET)
find_package(Espeak QUIET)

# Always build YARP devices as MODULE libraries.
set(YARP_FORCE_DYNAMIC_PLUGINS TRUE CACHE INTERNAL "Force dynamic plugins")

# Configure installation paths for YARP resources.
yarp_configure_external_installation(roboticslab-speech WITH_PLUGINS)

# Standard installation directories.
include(GNUInstallDirs)

# Control where libraries and executables are placed during the build.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

# Enabling coverage.
option(ENABLE_coverage "Choose if you want to enable coverage collection" OFF)

if(ENABLE_coverage)
    # List supported compilers.
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif()
endif()

# Create targets if specific requirements are satisfied.
include(CMakeDependentOption)

# Add main contents.
add_subdirectory(libraries)
add_subdirectory(programs)
#add_subdirectory(tests)
add_subdirectory(share)
add_subdirectory(doc)
add_subdirectory(examples/cpp)

# Store the package in the user registry.
set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)

# Create and install config files.
include(InstallBasicPackageFiles)

# Retrieve global properties.
get_property(_idl_includes GLOBAL PROPERTY ROBOTICSLAB_SPEECH_IDL_INCLUDES)
get_property(_idl_sources GLOBAL PROPERTY ROBOTICSLAB_SPEECH_IDL_SOURCES)

# Set exported variables (build tree).
set(ROBOTICSLAB_SPEECH_BUILD_IDL_INCLUDE ${_idl_includes})
set(ROBOTICSLAB_SPEECH_BUILD_IDL_SOURCE ${_idl_sources})

# Set exported variables (install tree).
set(ROBOTICSLAB_SPEECH_INSTALL_IDL_INCLUDE ${ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR}/idls)
set(ROBOTICSLAB_SPEECH_INSTALL_IDL_SOURCE "")

foreach(_file ${_idl_sources})
    get_filename_component(_filename ${_file} NAME)
    list(APPEND ROBOTICSLAB_SPEECH_INSTALL_IDL_SOURCE ${ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR_FULL}/idls/${_filename})
endforeach()

install_basic_package_files(ROBOTICSLAB_SPEECH
                            VERSION 0.1.0
                            COMPATIBILITY AnyNewerVersion
                            NO_EXPORT
                            NO_CHECK_REQUIRED_COMPONENTS_MACRO
                            NAMESPACE ROBOTICSLAB::
                            EXTRA_PATH_VARS_SUFFIX IDL_INCLUDE IDL_SOURCE
                            CONFIG_TEMPLATE ${CMAKE_SOURCE_DIR}/cmake/templates/ROBOTICSLAB_SPEECHConfig.cmake.in)

# Configure and create uninstall target.
include(AddUninstallTarget)
