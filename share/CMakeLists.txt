if(ENABLE_PiperSynthesizer)
    yarp_install(DIRECTORY PiperSynthesizer
                 DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR})
endif()

if(ENABLE_speechRecognition)
    yarp_install(DIRECTORY speechRecognition
                 DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR})
endif()

if(ENABLE_speechSynthesis)
    yarp_install(DIRECTORY speechSynthesis
                 DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR})
endif()

yarp_install(FILES applications/ymanager.ini
                   applications/example_espeakServer.xml
                   applications/example_speechRecognition.xml
             DESTINATION ${ROBOTICSLAB-SPEECH_APPLICATIONS_INSTALL_DIR})
