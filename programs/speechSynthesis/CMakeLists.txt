option(ENABLE_speechSynthesis "Install speechSynthesis program" ON)

if(ENABLE_speechSynthesis)

    install(PROGRAMS speechSynthesis.py
            TYPE BIN
            RENAME speechSynthesis)

endif()
