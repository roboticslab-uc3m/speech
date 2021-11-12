yarp_install(FILES applications/ymanager.ini
                   applications/example_espeakServer.xml
                   applications/example_speechRecognition.xml
             DESTINATION ${ROBOTICSLAB-SPEECH_APPLICATIONS_INSTALL_DIR})

add_subdirectory(speechRecognition)
