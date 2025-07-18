# FIXME: workaround for YARP 3.12.
if(YARP_VERSION VERSION_LESS 3.11.100)
    set(ALLOW_DEVICE_PARAM_PARSER_GENERATION_DEFAULT_VALUE OFF)
else()
    set(ALLOW_DEVICE_PARAM_PARSER_GENERATION_DEFAULT_VALUE ON)
endif()

option(ALLOW_DEVICE_PARAM_PARSER_GENERATION "Allow YARP to (re)build device param parsers"
       ${ALLOW_DEVICE_PARAM_PARSER_GENERATION_DEFAULT_VALUE})

add_subdirectory(eSpeakSynthesizer)
add_subdirectory(PiperSynthesizer)
add_subdirectory(VoskTranscription)
