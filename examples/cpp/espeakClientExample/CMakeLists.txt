cmake_minimum_required(VERSION 3.19..3.28)

project(espeakClientExample LANGUAGES CXX)

if(NOT YARP_FOUND)
    find_package(YARP 3.11 REQUIRED COMPONENTS os)
endif()

if(NOT TARGET ROBOTICSLAB::SpeechIDL)
    find_package(ROBOTICSLAB_SPEECH REQUIRED)
endif()

add_executable(espeakClientExample main.cpp)

target_link_libraries(espeakClientExample YARP::YARP_os
                                          YARP::YARP_init
                                          ROBOTICSLAB::SpeechIDL)
