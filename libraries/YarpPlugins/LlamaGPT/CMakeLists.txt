if(NOT llama_FOUND AND (NOT DEFINED ENABLE_LlamaGPT OR ENABLE_LlamaGPT))
    message(WARNING "llama package not found, disabling LlamaGPT")
endif()

yarp_prepare_plugin(LlamaGPT
                    CATEGORY device
                    TYPE LlamaGPT
                    INCLUDE LlamaGPT.hpp
                    DEFAULT ON
                    DEPENDS llama_FOUND
                    EXTRA_CONFIG WRAPPER=LLM_nws_yarp
                    GENERATE_PARSER)

if(NOT SKIP_LlamaGPT)

    yarp_add_plugin(LlamaGPT)

    target_sources(LlamaGPT PRIVATE LlamaGPT.hpp
                                    LlamaGPT.cpp
                                    DeviceDriverImpl.cpp
                                    ILLMImpl.cpp
                                    LlamaGPT_ParamsParser.h
                                    LlamaGPT_ParamsParser.cpp
                                    LogComponent.hpp
                                    LogComponent.cpp)

    target_link_libraries(LlamaGPT YARP::YARP_os
                                   YARP::YARP_dev
                                   llama)

    yarp_install(TARGETS LlamaGPT
                 LIBRARY DESTINATION ${ROBOTICSLAB-SPEECH_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-SPEECH_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-SPEECH_PLUGIN_MANIFESTS_INSTALL_DIR})

else()

    set(ENABLE_LlamaGPT OFF CACHE BOOL "Enable/disable LlamaGPT device" FORCE)

endif()
