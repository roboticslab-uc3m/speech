# Copyright: UC3M 2017
# Author: Raul de Santos Rico TEMPLATE_AUTHOR Juan Gonzalez Victores
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
# 

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpInstallationHelpers)
yarp_configure_external_installation(roboticslab-speech)

### Go through single applications

#add_subdirectory(prog)

