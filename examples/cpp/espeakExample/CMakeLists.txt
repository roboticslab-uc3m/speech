cmake_minimum_required(VERSION 3.16..3.28)

project(espeakExample LANGUAGES CXX)

if(NOT YARP_FOUND)
    find_package(YARP 3.8 REQUIRED COMPONENTS os)
endif()

if(NOT TARGET ROBOTICSLAB::Espeak)
    find_package(ROBOTICSLAB_SPEECH REQUIRED)
endif()

add_executable(espeakExample main.cpp)

target_link_libraries(espeakExample YARP::YARP_os
                                    ROBOTICSLAB::Espeak)
