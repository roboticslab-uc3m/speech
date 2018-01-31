cmake_minimum_required (VERSION 2.8.11)

project(espeakExample)

find_package(YARP REQUIRED)
find_package(ROBOTICSLAB_SPEECH REQUIRED)

include_directories(${YARP_INCLUDE_DIRS}
                    ${ROBOTICSLAB_SPEECH_IDL_INCLUDES})

add_executable(espeakExample main.cpp
                             EspeakExample.hpp
                             EspeakExample.cpp
                             ${ROBOTICSLAB_SPEECH_IDL_SOURCES})

target_link_libraries(espeakExample YARP::YARP_OS
                                    YARP::YARP_dev
                                    YARP::YARP_init)

include(GNUInstallDirs)

install(TARGETS espeakExample
        DESTINATION ${CMAKE_INSTALL_BINDIR})
