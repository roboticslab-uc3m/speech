option(ENABLE_speechRecognition "Install speechRecognition program" ON)

if(ENABLE_speechRecognition)

    install(PROGRAMS speechRecognition.py
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            RENAME speechRecognition)

endif()
