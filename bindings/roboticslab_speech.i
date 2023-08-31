%module "roboticslab_speech"

// enable polymorphism in Python-derived classes
%module(directors="1") "roboticslab_speech"
%feature("director") roboticslab::SpeechSynthesis;
%feature("director") roboticslab::SpeechRecognition;

// correctly handle std::int16_t and std::vector<std::string> (in help() method)
%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"

// avoid issues with some macros, e.g. YARP_os_API, YARP_DEPRECATED_MSG
%include "yarp/conf/api.h"
%include "yarp/os/api.h"
%include "yarp/os/Type.h" // avoid SWIG memory leaks due to a missing destructor

// we need this to preserve the inheritance chain: SpeechIDL < Wire < PortReader
%import(module="yarp") "yarp/os/PortReader.h"
%import(module="yarp") "yarp/os/Wire.h"

%{
#include "yarp/os/Type.h" // incomplete type due to a forward declaration in PortReader.h
#include "SpeechSynthesis.h"
#include "SpeechRecognition.h"
%}

%include "SpeechSynthesis.h"
%include "SpeechRecognition.h"
