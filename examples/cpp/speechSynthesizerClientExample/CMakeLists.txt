cmake_minimum_required(VERSION 3.19..3.28)

project(speechSynthesizerClientExample LANGUAGES CXX)

if(NOT YARP_FOUND)
    find_package(YARP 3.11 REQUIRED COMPONENTS os dev)
endif()

add_executable(speechSynthesizerClientExample main.cpp)

target_link_libraries(speechSynthesizerClientExample YARP::YARP_os
                                                     YARP::YARP_init
                                                     YARP::YARP_dev)
