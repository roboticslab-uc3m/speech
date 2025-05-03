// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "VoskTranscription.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

// ------------------- ISpeechTranscription Related ------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue VoskTranscription::setLanguage(const std::string & language)
#else
bool VoskTranscription::setLanguage(const std::string & language)
#endif
{
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue VoskTranscription::getLanguage(std::string & language)
#else
bool VoskTranscription::getLanguage(std::string & language)
#endif
{
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue VoskTranscription::transcribe(const yarp::sig::Sound & sound, std::string & transcription, double & score)
#else
bool VoskTranscription::transcribe(const yarp::sig::Sound & sound, std::string & transcription, double & score)
#endif
{
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------
