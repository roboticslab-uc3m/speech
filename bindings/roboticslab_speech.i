%module "roboticslab_speech"

// enable polymorphism in Python-derived classes
%module(directors="1") "roboticslab_speech"
%feature("director") roboticslab::TextToSpeechIDL;
%feature("director") roboticslab::SpeechRecognitionIDL;

// correctly handle std::int16_t and std::vector<std::string> (in help() method)
%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"

// avoid issues with some macros, e.g. YARP_os_API, YARP_DEPRECATED_MSG
%include "yarp/conf/api.h"
%include "yarp/os/api.h"

// we need this to preserve the inheritance chain: SpeechIDL < Wire < PortReader
%import(module="yarp") "yarp/os/PortReader.h"
%import(module="yarp") "yarp/os/Wire.h"

%{
#include "yarp/os/Type.h" // incomplete type due to a forward declaration in PortReader.h
#include "TextToSpeechIDL.h"
#include "SpeechRecognitionIDL.h"
%}

%include "TextToSpeechIDL.h"
%include "SpeechRecognitionIDL.h"
