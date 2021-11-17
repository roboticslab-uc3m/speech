%module "roboticslab_speech"

// correctly handle std::int16_t and std::string
%include "stdint.i"
%include "std_string.i"

// avoid issues with some macros, e.g. YARP_os_API, YARP_DEPRECATED_MSG
%include "yarp/conf/api.h"
%include "yarp/os/api.h"

// we need this to preserve the inheritance chain: SpeechIDL < Wire < PortReader
%import(module="yarp") "yarp/os/PortReader.h"
%import(module="yarp") "yarp/os/Wire.h"

%{
#include "TextToSpeechIDL.h"
%}

%include "TextToSpeechIDL.h"
