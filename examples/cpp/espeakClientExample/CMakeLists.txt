cmake_minimum_required(VERSION 3.19..3.28)

project(espeakClientExample LANGUAGES CXX)

if(NOT YARP_FOUND)
    set(_yarp_components os init)
    find_package(YARP 3.12 QUIET COMPONENTS ${_yarp_components})

    if(NOT YARP_FOUND)
        find_package(YARP 4.0 REQUIRED COMPONENTS ${_yarp_components})
    endif()
endif()

if(NOT TARGET ROBOTICSLAB::SpeechIDL)
    find_package(ROBOTICSLAB_SPEECH REQUIRED)
endif()

add_executable(espeakClientExample main.cpp)

target_link_libraries(espeakClientExample YARP::YARP_os
                                          YARP::YARP_init
                                          ROBOTICSLAB::SpeechIDL)
