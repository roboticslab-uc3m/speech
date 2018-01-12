# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Authors: Juan G. Victores & Raul de Santos Rico
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6)

#IF (ENABLE_YarpPlugins)

  option(ENABLE_launchYarpPlugins "Choose if you want to compile launchYarpPlugins" TRUE)

  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})  
  include(YarpPlugin)

  add_subdirectory(Espeak)

#ENDIF (ENABLE_YarpPlugins)
