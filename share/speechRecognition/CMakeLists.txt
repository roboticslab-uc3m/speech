yarp_install(FILES speechRecognition.ini
             DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR}/speechRecognition)

yarp_install(DIRECTORY dictionary
                       model
             DESTINATION ${ROBOTICSLAB-SPEECH_CONTEXTS_INSTALL_DIR}/speechRecognition)
