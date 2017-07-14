# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Author: Juan G. Victores & Raul de Santos Rico
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

YARP_PREPARE_PLUGIN(Espeak CATEGORY device TYPE roboticslab::Espeak INCLUDE Espeak.hpp)

IF (NOT SKIP_Espeak)

SET(CMAKE_MODULE_PATH ${ROBOTICSLAB_SPEECH_MODULE_PATH} ${CMAKE_MODULE_PATH})
FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(Espeak REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${ROBOTICSLAB_SPEECH_INCLUDE_DIRS} ${ESPEAK_INCLUDE_DIRS})

yarp_add_idl(IDL_GEN_FILES speech.thrift) # calls include_directories() on generated headers' dir

YARP_ADD_PLUGIN(Espeak Espeak.hpp Espeak.cpp ${IDL_GEN_FILES})
TARGET_LINK_LIBRARIES(Espeak ${YARP_LIBRARIES} ${ESPEAK_LIBRARIES})

get_directory_property(_included_dirs INCLUDE_DIRECTORIES)

# Exporting dependencies for ROBOTICSLAB_SPEECHConfig.cmake quite manually for now... 
set(ROBOTICSLAB_SPEECH_INCLUDE_DIRS ${ROBOTICSLAB_SPEECH_INCLUDE_DIRS} ${_included_dirs}  CACHE INTERNAL "appended headers")

#install(TARGETS Espeak DESTINATION lib)
yarp_install(TARGETS Espeak
     EXPORT YARP
     COMPONENT runtime
     LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
     ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

yarp_install(FILES Espeak.ini
     COMPONENT runtime
     DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

# already included in /share?
if(NOT DEFINED ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR)
  include(YarpInstallationHelpers)
  yarp_configure_external_installation(roboticslab-speech)
endif()

yarp_install(FILES speech.thrift
             DESTINATION ${ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR})

# points at current build dir, should be fixed along with <pkg>Config.cmake to handle the install tree, too
set(ROBOTICSLAB_SPEECH_DATA_PATH "${CMAKE_BINARY_DIR}/${ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR}" CACHE INTERNAL "data path (e.g. thrift files)")

ENDIF (NOT SKIP_Espeak)

