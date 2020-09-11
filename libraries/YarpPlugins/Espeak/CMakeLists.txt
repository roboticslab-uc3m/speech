# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Author: Juan G. Victores & Raul de Santos Rico
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(NOT DEFINED ENABLE_Espeak OR ENABLE_Espeak)
    if(NOT Espeak_FOUND)
        message(WARNING "Espeak package not found, disabling Espeak device")
    endif()

    if(NOT YARP_idl_tools_FOUND)
        message(WARNING "YARP idl_tools component not found, disabling Espeak device")
    endif()
endif()

yarp_prepare_plugin(Espeak
                    CATEGORY device
                    TYPE roboticslab::Espeak
                    INCLUDE Espeak.hpp
                    DEFAULT ON
                    DEPENDS "Espeak_FOUND;YARP_idl_tools_FOUND")

if(NOT SKIP_Espeak)

    yarp_idl_to_dir(INPUT_FILES speech.thrift
                    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
                    SOURCES_VAR idl_sources
                    HEADERS_VAR idl_headers
                    INCLUDE_DIRS_VAR idl_include_dirs)

    if(NOT YARP_VERSION VERSION_GREATER_EQUAL 3.4)
        set(CMAKE_INCLUDE_CURRENT_DIR TRUE) # yarp plugin builder needs this
    endif()

    yarp_add_plugin(Espeak Espeak.hpp
                           Espeak.cpp
                           ${idl_sources}
                           ${idl_headers})

    target_link_libraries(Espeak YARP::YARP_OS
                                 YARP::YARP_dev
                                 ${ESPEAK_LIBRARIES}
                                 ROBOTICSLAB::ColorDebug)

    target_include_directories(Espeak PRIVATE ${ESPEAK_INCLUDE_DIRS}
                                              ${idl_include_dirs})

    yarp_install(TARGETS Espeak
                 LIBRARY DESTINATION ${ROBOTICSLAB-SPEECH_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-SPEECH_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-SPEECH_PLUGIN_MANIFESTS_INSTALL_DIR})

    yarp_install(FILES ${idl_sources}
                       ${idl_headers}
                 DESTINATION ${ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR}/idls)

    foreach(_var_name IN ITEMS idl_sources idl_headers)
        foreach(_var_value IN LISTS ${_var_name})
            get_filename_component(${_var_name} ${_var_value} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/..)
        endforeach()
    endforeach()

    set_property(GLOBAL APPEND PROPERTY ROBOTICSLAB_SPEECH_IDL_INCLUDES ${idl_include_dirs})
    set_property(GLOBAL APPEND PROPERTY ROBOTICSLAB_SPEECH_IDL_SOURCES ${idl_sources} ${idl_headers})

else()

    set(ENABLE_Espeak OFF CACHE BOOL "Enable/disable Espeak device" FORCE)

endif()
