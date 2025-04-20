if(NOT Espeak_FOUND AND (NOT DEFINED ENABLE_eSpeakSynthesizer OR ENABLE_eSpeakSynthesizer))
    message(WARNING "Espeak package not found, disabling eSpeakSynthesizer")
endif()

yarp_prepare_plugin(eSpeakSynthesizer
                    CATEGORY device
                    TYPE eSpeakSynthesizer
                    INCLUDE eSpeakSynthesizer.hpp
                    DEFAULT ON
                    DEPENDS Espeak_FOUND
                    EXTRA_CONFIG WRAPPER=speechSynthesizer_nws_yarp
                    GENERATE_PARSER)

if(NOT SKIP_eSpeakSynthesizer)

    yarp_add_plugin(eSpeakSynthesizer)

    target_sources(eSpeakSynthesizer PRIVATE eSpeakSynthesizer.hpp
                                             DeviceDriverImpl.cpp
                                             ISpeechSynthesizerImpl.cpp
                                             eSpeakSynthesizer_ParamsParser.h
                                             eSpeakSynthesizer_ParamsParser.cpp
                                             LogComponent.hpp
                                             LogComponent.cpp)

    target_link_libraries(eSpeakSynthesizer YARP::YARP_os
                                            YARP::YARP_dev
                                            Espeak::Espeak)

    yarp_install(TARGETS eSpeakSynthesizer
                 LIBRARY DESTINATION ${ROBOTICSLAB-SPEECH_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-SPEECH_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-SPEECH_PLUGIN_MANIFESTS_INSTALL_DIR})

else()

    set(ENABLE_eSpeakSynthesizer OFF CACHE BOOL "Enable/disable eSpeakSynthesizer device" FORCE)

endif()
