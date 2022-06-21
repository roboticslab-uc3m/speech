if(NOT YARP_idl_tools_FOUND AND (NOT DEFINED ENABLE_SpeechIDL OR ENABLE_SpeechIDL))
    message(WARNING "YARP idl_tools component not found, disabling SpeechIDL library")
endif()

cmake_dependent_option(ENABLE_SpeechIDL "Enable/disable SpeechIDL library" ON
                       YARP_idl_tools_FOUND OFF)

if(ENABLE_SpeechIDL)

    set(ALLOW_IDL_GENERATION ON CACHE BOOL "Detect changes and rebuild IDL files")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS speech.thrift)

    yarp_idl_to_dir(INPUT_FILES speech.thrift
                    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
                    SOURCES_VAR idl_sources
                    HEADERS_VAR idl_headers
                    INCLUDE_DIRS_VAR idl_include_dirs
                    PLACEMENT MERGED
                    THRIFT_NO_NAMESPACE_PREFIX)

    add_library(SpeechIDL SHARED ${idl_sources} ${idl_headers})

    set_target_properties(SpeechIDL PROPERTIES PUBLIC_HEADER "${idl_headers}")

    target_link_libraries(SpeechIDL PUBLIC YARP::YARP_os)

    target_include_directories(SpeechIDL PUBLIC $<BUILD_INTERFACE:${idl_include_dirs}>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    install(TARGETS SpeechIDL
            EXPORT ROBOTICSLAB_SPEECH)

    set_property(GLOBAL APPEND PROPERTY _exported_targets SpeechIDL)

    add_library(ROBOTICSLAB::SpeechIDL ALIAS SpeechIDL)

else()

    set(ENABLE_SpeechIDL OFF CACHE BOOL "Enable/disable SpeechIDL library" FORCE)

endif()
