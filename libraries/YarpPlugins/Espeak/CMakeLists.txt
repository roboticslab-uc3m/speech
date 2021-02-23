# Copyright: (C) 2021 Universidad Carlos III de Madrid
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

    add_library(SpeechIDL INTERFACE)

    set(_idl_interface_sources)

    foreach(_var_name IN ITEMS idl_sources idl_headers)
        foreach(_var_file IN LISTS ${_var_name})
            get_filename_component(_temp_build ${_var_file} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/..)
            get_filename_component(_temp_install ${_var_file} NAME)

            list(APPEND _idl_interface_sources $<BUILD_INTERFACE:${_temp_build}>
                                               $<INSTALL_INTERFACE:${ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR}/idls/${_temp_install}>)
        endforeach()
    endforeach()

    target_sources(SpeechIDL INTERFACE ${_idl_interface_sources})

    target_include_directories(SpeechIDL INTERFACE $<BUILD_INTERFACE:${idl_include_dirs}>
                                                   $<INSTALL_INTERFACE:${ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR}/idls>)

    yarp_install(TARGETS SpeechIDL
                 EXPORT ROBOTICSLAB_SPEECH)

    yarp_install(FILES ${idl_sources}
                       ${idl_headers}
             DESTINATION ${ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR}/idls)

    set_property(GLOBAL APPEND PROPERTY _exported_targets SpeechIDL)

    add_library(ROBOTICSLAB::SpeechIDL ALIAS SpeechIDL)

    if(NOT YARP_VERSION VERSION_GREATER_EQUAL 3.4)
        set(CMAKE_INCLUDE_CURRENT_DIR TRUE) # yarp plugin builder needs this
    endif()

    yarp_add_plugin(Espeak Espeak.hpp
                           Espeak.cpp)

    target_link_libraries(Espeak YARP::YARP_os
                                 YARP::YARP_dev
                                 ROBOTICSLAB::SpeechIDL
                                 ${ESPEAK_LIBRARIES})

    target_include_directories(Espeak PRIVATE ${ESPEAK_INCLUDE_DIRS})

    yarp_install(TARGETS Espeak
                 LIBRARY DESTINATION ${ROBOTICSLAB-SPEECH_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-SPEECH_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-SPEECH_PLUGIN_MANIFESTS_INSTALL_DIR})

else()

    set(ENABLE_Espeak OFF CACHE BOOL "Enable/disable Espeak device" FORCE)

endif()
