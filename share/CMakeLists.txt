# Copyright: 2017 UC3M
# Author: Juan G Victores & Raul de santos Rico
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_install(FILES ymanager.ini
             DESTINATION ${ROBOTICSLAB-SPEECH_DATA_INSTALL_DIR})

### Go through single applications

add_subdirectory(speechRecognition)
