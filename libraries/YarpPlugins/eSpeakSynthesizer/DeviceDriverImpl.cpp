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

    if (m_bufLength < 0)
    {
        yCError(ESS) << "Buffer length must be greater than 0";
        return false;
    }

    const auto * version = espeak_Info(nullptr);
    yCInfo(ESS) << "eSpeak version:" << version;

    if ((sampleRate = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, m_bufLength, nullptr, 0)) == EE_INTERNAL_ERROR)
    {
        yCError(ESS) << "espeak_Initialize() failed";
        return false;
    }

    yCInfo(ESS) << "Sample rate:" << sampleRate << "Hz";

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
