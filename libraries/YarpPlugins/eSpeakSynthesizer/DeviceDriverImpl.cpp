// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "eSpeakSynthesizer.hpp"

#include <yarp/os/LogStream.h>

#include <espeak/speak_lib.h>

#include "LogComponent.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool eSpeakSynthesizer::open(yarp::os::Searchable & config)
{
    espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_PLAYBACK;
    int buflength = 500;
    char * path = nullptr;
    int options = 0;

    if (espeak_Initialize(output, buflength, path, options) == EE_INTERNAL_ERROR)
    {
        yCError(ESS) << "espeak_Initialize() failed";
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool eSpeakSynthesizer::close()
{
    if (espeak_Terminate() != EE_OK)
    {
        yCError(ESS) << "espeak_Terminate() failed";
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
