cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(espeakExample LANGUAGES CXX)

if(NOT YARP_FOUND)
    find_package(YARP 3.3 REQUIRED COMPONENTS os dev)
endif()

if(NOT ROBOTICSLAB_SPEECH_FOUND)
    find_package(ROBOTICSLAB_SPEECH REQUIRED)
endif()

add_executable(espeakExample main.cpp
                             EspeakExample.hpp
                             EspeakExample.cpp
                             ${ROBOTICSLAB_SPEECH_IDL_SOURCES})

target_link_libraries(espeakExample YARP::YARP_os
                                    YARP::YARP_dev
                                    YARP::YARP_init)

target_include_directories(espeakExample PRIVATE ${ROBOTICSLAB_SPEECH_IDL_INCLUDES})
