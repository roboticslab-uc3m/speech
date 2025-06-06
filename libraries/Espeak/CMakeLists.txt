if(NOT Espeak_FOUND AND (NOT DEFINED ENABLE_Espeak OR ENABLE_Espeak))
    message(WARNING "Espeak package not found, disabling Espeak library")
endif()

cmake_dependent_option(ENABLE_Espeak "Enable/disable Espeak library" ON
                       "Espeak_FOUND;ENABLE_SpeechIDL" OFF)

if(ENABLE_Espeak)

    add_library(Espeak SHARED Espeak.hpp
                              Espeak.cpp)

    set_target_properties(Espeak PROPERTIES PUBLIC_HEADER Espeak.hpp)

    target_link_libraries(Espeak PUBLIC ROBOTICSLAB::SpeechIDL
                                 PRIVATE YARP::YARP_os
                                         Espeak::Espeak)

    target_include_directories(Espeak PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                             $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    install(TARGETS Espeak
            EXPORT ROBOTICSLAB_SPEECH)

    set_property(GLOBAL APPEND PROPERTY _exported_targets Espeak)

    add_library(ROBOTICSLAB::Espeak ALIAS Espeak)

else()

    set(ENABLE_Espeak OFF CACHE BOOL "Enable/disable Espeak library" FORCE)

endif()
