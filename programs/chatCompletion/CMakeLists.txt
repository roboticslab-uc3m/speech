option(ENABLE_chatCompletion "Install chatCompletion program" ON)

if(ENABLE_chatCompletion)

    install(PROGRAMS chatCompletion.py
            TYPE BIN
            RENAME chatCompletion)

endif()
