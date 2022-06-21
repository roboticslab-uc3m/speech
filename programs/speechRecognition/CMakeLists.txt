option(ENABLE_speechRecognition "Install speechRecognition program" ON)

if(ENABLE_speechRecognition)

    install(PROGRAMS speechRecognition.py
            TYPE BIN
            RENAME speechRecognition)

endif()
