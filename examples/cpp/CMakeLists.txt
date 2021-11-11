option(ENABLE_examples "Enable/disable C++ examples" OFF)

if(ENABLE_examples)
    if(ENABLE_Espeak)
        add_subdirectory(espeakExample)
        add_subdirectory(espeakClientExample)
    endif()
endif()
