if(NOT Piper_FOUND AND (NOT DEFINED ENABLE_PiperSynthesizer OR ENABLE_PiperSynthesizer))
    message(WARNING "Piper package not found, disabling PiperSynthesizer")
endif()

yarp_prepare_plugin(PiperSynthesizer
                    CATEGORY device
                    TYPE PiperSynthesizer
                    INCLUDE PiperSynthesizer.hpp
                    DEFAULT ON
                    DEPENDS Piper_FOUND
                    EXTRA_CONFIG WRAPPER=speechSynthesizer_nws_yarp
                    GENERATE_PARSER)

if(NOT SKIP_PiperSynthesizer)

    yarp_add_plugin(PiperSynthesizer)

    target_sources(PiperSynthesizer PRIVATE PiperSynthesizer.hpp
                                            DeviceDriverImpl.cpp
                                            ISpeechSynthesizerImpl.cpp
                                            PiperSynthesizer_ParamsParser.h
                                            PiperSynthesizer_ParamsParser.cpp
                                            LogComponent.hpp
                                            LogComponent.cpp)

    target_link_libraries(PiperSynthesizer YARP::YARP_os
                                           YARP::YARP_dev
                                           Piper::piper_lib)

    target_compile_definitions(PiperSynthesizer PRIVATE _PIPER_DATA_DIR="${PIPER_DATA_DIR}")

    yarp_install(TARGETS PiperSynthesizer
                 LIBRARY DESTINATION ${ROBOTICSLAB-SPEECH_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-SPEECH_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-SPEECH_PLUGIN_MANIFESTS_INSTALL_DIR})

else()

    set(ENABLE_PiperSynthesizer OFF CACHE BOOL "Enable/disable PiperSynthesizer device" FORCE)

endif()
