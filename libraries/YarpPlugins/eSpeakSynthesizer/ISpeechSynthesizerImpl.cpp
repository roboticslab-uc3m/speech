// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "eSpeakSynthesizer.hpp"

#include <algorithm> // std::clamp

#include <yarp/os/LogStream.h>

#include <espeak/speak_lib.h>

#include "LogComponent.hpp"

using namespace roboticslab;

namespace
{
    constexpr auto MIN_SPEED = 80;
    constexpr auto MAX_SPEED = 450;

    inline int userToLibSpeed(double userSpeed)
    {
        return static_cast<int>(MIN_SPEED + (MAX_SPEED - MIN_SPEED) * userSpeed);
    }

    inline double libToUserSpeed(int libSpeed)
    {
        return static_cast<double>(libSpeed - MIN_SPEED) / (MAX_SPEED - MIN_SPEED);
    }

    inline int userToLibPitch(double userPitch)
    {
        return static_cast<int>(userPitch * 100.0);
    }

    inline double libToUserPitch(int libPitch)
    {
        return static_cast<double>(libPitch) / 100.0;
    }
}

// ------------------- ISpeechSynthesizer Related ------------------------------------

bool eSpeakSynthesizer::setLanguage(const std::string & language)
{
    yCInfo(ESS) << "Setting language to:" << language;
    espeak_VOICE voice_spec = {};
    voice_spec.languages = language.c_str();
    return espeak_SetVoiceByProperties(&voice_spec) == EE_OK;
}

// -----------------------------------------------------------------------------

bool eSpeakSynthesizer::getLanguage(std::string & language)
{
    const auto * voice = espeak_GetCurrentVoice();
    language = voice->languages;
    return true;
}

// -----------------------------------------------------------------------------

bool eSpeakSynthesizer::setVoice(const std::string & voice_name)
{
    yCInfo(ESS) << "Setting voice to:" << voice_name;
    return espeak_SetVoiceByName(voice_name.c_str()) == EE_OK;
}

// -----------------------------------------------------------------------------

bool eSpeakSynthesizer::getVoice(std::string & voice_name)
{
    const auto * voice = espeak_GetCurrentVoice();
    voice_name = voice->name;
    return true;
}

// -----------------------------------------------------------------------------

bool eSpeakSynthesizer::setSpeed(const double speed)
{
    auto clamped = std::clamp(speed, 0.0, 1.0);
    yCInfo(ESS) << "Setting speed to:" << clamped;
    return espeak_SetParameter(espeakRATE, userToLibSpeed(clamped), 0) == EE_OK;
}

// -----------------------------------------------------------------------------

bool eSpeakSynthesizer::getSpeed(double & speed)
{
    auto original = espeak_GetParameter(espeakRATE, 1);
    speed = libToUserSpeed(original);
    return true;
}

// -----------------------------------------------------------------------------

bool eSpeakSynthesizer::setPitch(const double pitch)
{
    auto clamped = std::clamp(pitch, 0.0, 1.0);
    yCInfo(ESS) << "Setting pitch to:" << clamped;
    return espeak_SetParameter(espeakPITCH, userToLibPitch(clamped), 0) == EE_OK;
}

// -----------------------------------------------------------------------------

bool eSpeakSynthesizer::getPitch(double & pitch)
{
    auto original = espeak_GetParameter(espeakPITCH, 1);
    pitch = libToUserPitch(original);
    return true;
}

// -----------------------------------------------------------------------------

bool eSpeakSynthesizer::synthesize(const std::string & text, yarp::sig::Sound & sound)
{
    return false;
}

// -----------------------------------------------------------------------------
