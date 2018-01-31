option(ENABLE_speechRecognition "Install speechRecognition program" ON)

if(ENABLE_speechRecognition)

yarp_install(PROGRAMS speechRecognition.py
             DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
