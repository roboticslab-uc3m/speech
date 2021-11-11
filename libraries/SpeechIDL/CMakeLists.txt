if(NOT YARP_idl_tools_FOUND AND (NOT DEFINED ENABLE_SpeechIDL OR ENABLE_SpeechIDL))
    message(WARNING "YARP idl_tools component not found, disabling SpeechIDL library")
endif()

cmake_dependent_option(ENABLE_SpeechIDL "Enable/disable SpeechIDL library" ON
                       YARP_idl_tools_FOUND OFF)

if(ENABLE_SpeechIDL)

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

else()

    set(ENABLE_SpeechIDL OFF CACHE BOOL "Enable/disable SpeechIDL library" FORCE)

endif()
