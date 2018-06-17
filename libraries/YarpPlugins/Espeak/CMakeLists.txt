# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Author: Juan G. Victores & Raul de Santos Rico
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

find_package(Espeak QUIET)

if(NOT Espeak_FOUND AND (NOT DEFINED ENABLE_Espeak OR ENABLE_Espeak))
    if(NOT Espeak_FOUND)
        message(WARNING "Espeak package not found, disabling Espeak device")
    endif()

    if(NOT YARP_HAS_IDL)
        message(WARNING "YARP IDL modules not found, disabling Espeak device")
    endif()
endif()

yarp_prepare_plugin(Espeak
                    CATEGORY device
                    TYPE roboticslab::Espeak
                    INCLUDE Espeak.hpp
                    DEFAULT ON
                    DEPENDS "Espeak_FOUND;YARP_HAS_IDL")

if(NOT SKIP_Espeak)

    yarp_idl_to_dir(speech.thrift ${CMAKE_CURRENT_BINARY_DIR} idl_sources idl_headers idl_include_paths)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR} # yarp plugin builder needs this
                        ${ESPEAK_INCLUDE_DIRS}
                        ${idl_include_paths})

    yarp_add_plugin(Espeak Espeak.hpp
                           Espeak.cpp
                           ${idl_sources}
                           ${idl_headers})

    target_link_libraries(Espeak YARP::YARP_OS
                                 YARP::YARP_dev
                                 YARP::YARP_sig
                                 ${ESPEAK_LIBRARIES}
                                 ROBOTICSLAB::ColorDebug)

    yarp_install(TARGETS Espeak
                 COMPONENT runtime
                 LIBRARY DESTINATION ${ROBOTICSLAB-SPEECH_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-SPEECH_STATIC_PLUGINS_INSTALL_DIR})

    yarp_install(FILES Espeak.ini
                 COMPONENT runtime
                 DESTINATION ${ROBOTICSLAB-SPEECH_PLUGIN_MANIFESTS_INSTALL_DIR})

    # Already included in /share?
    if(NOT DEFINED ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR)
        include(YarpInstallationHelpers)
        yarp_configure_external_installation(roboticslab-speech)
    endif()

    yarp_install(FILES ${thrift_files}
                       ${idl_sources}
                       ${idl_headers}
                 DESTINATION ${ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR}/idls)

    set_property(GLOBAL APPEND PROPERTY ROBOTICSLAB_SPEECH_IDL_INCLUDES ${idl_include_paths})
    set_property(GLOBAL APPEND PROPERTY ROBOTICSLAB_SPEECH_IDL_SOURCES ${idl_sources} ${idl_headers})

else()

    set(ENABLE_Espeak OFF CACHE BOOL "Enable/disable Espeak device" FORCE)

endif()
