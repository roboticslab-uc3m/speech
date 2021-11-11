cmake_minimum_required(VERSION 3.12)

project(espeakExample LANGUAGES CXX)

if(NOT YARP_FOUND)
    find_package(YARP 3.4 REQUIRED COMPONENTS os dev)
endif()

if(NOT TARGET ROBOTICSLAB::SpeechIDL)
    find_package(ROBOTICSLAB_SPEECH REQUIRED)
endif()

add_executable(espeakExample main.cpp)

target_link_libraries(espeakExample YARP::YARP_os
                                    YARP::YARP_dev
                                    YARP::YARP_init
                                    ROBOTICSLAB::SpeechIDL)
