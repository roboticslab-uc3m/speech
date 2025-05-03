if(NOT Vosk_FOUND AND (NOT DEFINED ENABLE_VoskTranscription OR ENABLE_VoskTranscription))
    message(WARNING "Vosk package not found, disabling VoskTranscription")
endif()

yarp_prepare_plugin(VoskTranscription
                    CATEGORY device
                    TYPE VoskTranscription
                    INCLUDE VoskTranscription.hpp
                    DEFAULT ON
                    DEPENDS Vosk_FOUND
                    EXTRA_CONFIG WRAPPER=speechTranscription_nws_yarp
                    GENERATE_PARSER)

if(NOT SKIP_VoskTranscription)

    yarp_add_plugin(VoskTranscription)

    target_sources(VoskTranscription PRIVATE VoskTranscription.hpp
                                             DeviceDriverImpl.cpp
                                             ISpeechTranscriptionImpl.cpp
                                             VoskTranscription_ParamsParser.h
                                             VoskTranscription_ParamsParser.cpp
                                             LogComponent.hpp
                                             LogComponent.cpp)

    target_link_libraries(VoskTranscription YARP::YARP_os
                                            YARP::YARP_dev
                                            Vosk::Vosk)

    yarp_install(TARGETS VoskTranscription
                 LIBRARY DESTINATION ${ROBOTICSLAB-SPEECH_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-SPEECH_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-SPEECH_PLUGIN_MANIFESTS_INSTALL_DIR})

else()

    set(ENABLE_VoskTranscription OFF CACHE BOOL "Enable/disable VoskTranscription device" FORCE)

endif()
