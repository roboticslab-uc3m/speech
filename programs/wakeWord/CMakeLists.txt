option(ENABLE_wakeWord "Install wakeWord program" ON)

if(ENABLE_wakeWord)

    install(PROGRAMS wakeWord.py
            TYPE BIN
            RENAME wakeWord)

endif()
