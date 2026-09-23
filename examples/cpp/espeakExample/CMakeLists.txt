cmake_minimum_required(VERSION 3.19..3.28)

project(espeakExample LANGUAGES CXX)

if(NOT YARP_FOUND)
    set(_yarp_components os init)
    find_package(YARP 3.12 QUIET COMPONENTS ${_yarp_components})

    if(NOT YARP_FOUND)
        find_package(YARP 4.0 REQUIRED COMPONENTS ${_yarp_components})
    endif()
endif()

if(NOT TARGET ROBOTICSLAB::Espeak)
    find_package(ROBOTICSLAB_SPEECH REQUIRED)
endif()

add_executable(espeakExample main.cpp)

target_link_libraries(espeakExample YARP::YARP_os
                                    ROBOTICSLAB::Espeak)
