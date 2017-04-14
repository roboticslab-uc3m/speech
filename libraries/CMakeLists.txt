# Copyright: Universidad Carlos III de Madrid (C) 2013
# Authors: Juan G. Victores & Raul de Santos Rico

set(COLOR_DEBUG_PART_OF_PROJECT TRUE)
add_subdirectory(ColorDebug) # ColorDebug sets COLOR_DEBUG_INCLUDE_DIRS
set(ROBOTICSLAB_SPEECH_INCLUDE_DIRS ${ROBOTICSLAB_SPEECH_INCLUDE_DIRS} ${COLOR_DEBUG_INCLUDE_DIRS})

add_subdirectory(SpeechYarp)  # SpeechYarp a plugin library which contains YARP plugins

