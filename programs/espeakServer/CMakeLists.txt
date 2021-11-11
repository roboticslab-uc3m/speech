cmake_dependent_option(ENABLE_espeakServer "Enable/disable espeakServer program" ON
                       "ENABLE_Espeak" OFF)

if(ENABLE_espeakServer)

    add_executable(espeakServer main.cpp
                                EspeakServer.hpp
                                EspeakServer.cpp)

    target_link_libraries(espeakServer YARP::YARP_os
                                       YARP::YARP_init
                                       ROBOTICSLAB::Espeak)

    install(TARGETS espeakServer
            DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
