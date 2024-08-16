yarp_prepare_plugin(eSpeakSynthesizer
                    CATEGORY device
                    TYPE roboticslab::eSpeakSynthesizer
                    INCLUDE eSpeakSynthesizer.hpp
                    DEFAULT ON
                    DEPENDS "YARP_VERSION VERSION_GREATER_EQUAL 3.9;Espeak_FOUND")

if(NOT SKIP_eSpeakSynthesizer)

    message("ESPEAK_LIBRARY: ${ESPEAK_LIBRARY}")

    yarp_add_plugin(eSpeakSynthesizer eSpeakSynthesizer.hpp
                                      DeviceDriverImpl.cpp
                                      ISpeechSynthesizerImpl.cpp
                                      LogComponent.hpp
                                      LogComponent.cpp)

    target_link_libraries(eSpeakSynthesizer YARP::YARP_os
                                            YARP::YARP_dev
                                            Espeak::Espeak)

    yarp_install(TARGETS eSpeakSynthesizer
                 LIBRARY DESTINATION ${ROBOTICSLAB-SPEECH_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-SPEECH_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-SPEECH_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
