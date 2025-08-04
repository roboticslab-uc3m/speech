if(ENABLE_PiperSynthesizer)
    yarp_install(DIRECTORY contexts/PiperSynthesizer
                 DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR})
endif()

if(ENABLE_VoskTranscription)
    yarp_install(DIRECTORY contexts/VoskTranscription
                 DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR})
endif()

if(ENABLE_speechRecognition)
    yarp_install(DIRECTORY contexts/speechRecognition
                 DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR})
endif()

if(ENABLE_speechSynthesis)
    yarp_install(DIRECTORY contexts/speechSynthesis
                 DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR})
endif()

yarp_install(FILES applications/ymanager.ini
                   applications/example_espeakServer.xml
                   applications/example_speechRecognition.xml
                   applications/speechSynthesis.xml
                   applications/speechTranscription.xml
             DESTINATION ${ROBOTICSLAB-SPEECH_APPLICATIONS_INSTALL_DIR})
