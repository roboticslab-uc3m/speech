// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "eSpeakSynthesizer.hpp"

#include <yarp/os/LogStream.h>

#include <espeak/speak_lib.h>

#include "LogComponent.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool eSpeakSynthesizer::open(yarp::os::Searchable & config)
{
    if (!parseParams(config))
    {
        yCError(ESS) << "Failed to parse parameters";
        return false;
    }

    const auto * version = espeak_Info(nullptr);
    yCInfo(ESS) << "eSpeak version:" << version;

    espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_PLAYBACK;
    int buflength = 500;
    char * path = nullptr;
    int options = 0;

    auto ret = espeak_Initialize(output, buflength, path, options);

    if (ret == EE_INTERNAL_ERROR)
    {
        yCError(ESS) << "espeak_Initialize() failed";
        return false;
    }

    yCInfo(ESS) << "Sample rate:" << ret << "Hz";

    if (!m_voice.empty() && !setVoice(m_voice))
    {
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
