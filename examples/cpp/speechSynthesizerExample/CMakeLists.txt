cmake_minimum_required(VERSION 3.19..3.28)

project(speechSynthesizerExample LANGUAGES CXX)

if(NOT YARP_FOUND)
    find_package(YARP 3.11 REQUIRED COMPONENTS os dev)
endif()

add_executable(speechSynthesizerExample main.cpp)

target_link_libraries(speechSynthesizerExample YARP::YARP_os
                                               YARP::YARP_init
                                               YARP::YARP_dev)
