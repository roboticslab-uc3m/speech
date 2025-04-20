// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "eSpeakSynthesizer.hpp"

#include <algorithm> // std::clamp

#include <yarp/os/LogStream.h>

#include <espeak/speak_lib.h>

#include "LogComponent.hpp"

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

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue eSpeakSynthesizer::setLanguage(const std::string & language)
#else
bool eSpeakSynthesizer::setLanguage(const std::string & language)
#endif
{
    yCInfo(ESS) << "Setting language to:" << language;
    espeak_VOICE voice_spec = {};
    voice_spec.languages = language.c_str();
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return espeak_SetVoiceByProperties(&voice_spec) == EE_OK
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_generic;
#else
    return espeak_SetVoiceByProperties(&voice_spec) == EE_OK;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue eSpeakSynthesizer::getLanguage(std::string & language)
#else
bool eSpeakSynthesizer::getLanguage(std::string & language)
#endif
{
    const auto * voice = espeak_GetCurrentVoice();
    language = voice->languages;
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue eSpeakSynthesizer::setVoice(const std::string & voice_name)
#else
bool eSpeakSynthesizer::setVoice(const std::string & voice_name)
#endif
{
    yCInfo(ESS) << "Setting voice to:" << voice_name;
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return espeak_SetVoiceByName(voice_name.c_str()) == EE_OK
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_generic;
#else
    return espeak_SetVoiceByName(voice_name.c_str()) == EE_OK;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue eSpeakSynthesizer::getVoice(std::string & voice_name)
#else
bool eSpeakSynthesizer::getVoice(std::string & voice_name)
#endif
{
    const auto * voice = espeak_GetCurrentVoice();
    voice_name = voice->name;
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue eSpeakSynthesizer::setSpeed(const double speed)
#else
bool eSpeakSynthesizer::setSpeed(const double speed)
#endif
{
    auto clamped = std::clamp(speed, 0.0, 1.0);
    yCInfo(ESS) << "Setting speed to:" << clamped;
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return espeak_SetParameter(espeakRATE, userToLibSpeed(clamped), 0) == EE_OK
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_generic;
#else
    return espeak_SetParameter(espeakRATE, userToLibSpeed(clamped), 0) == EE_OK;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue eSpeakSynthesizer::getSpeed(double & speed)
#else
bool eSpeakSynthesizer::getSpeed(double & speed)
#endif
{
    auto original = espeak_GetParameter(espeakRATE, 1);
    speed = libToUserSpeed(original);
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue eSpeakSynthesizer::setPitch(const double pitch)
#else
bool eSpeakSynthesizer::setPitch(const double pitch)
#endif
{
    auto clamped = std::clamp(pitch, 0.0, 1.0);
    yCInfo(ESS) << "Setting pitch to:" << clamped;
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return espeak_SetParameter(espeakPITCH, userToLibPitch(clamped), 0) == EE_OK
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_generic;
#else
    return espeak_SetParameter(espeakPITCH, userToLibPitch(clamped), 0) == EE_OK;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue eSpeakSynthesizer::getPitch(double & pitch)
#else
bool eSpeakSynthesizer::getPitch(double & pitch)
#endif
{
    auto original = espeak_GetParameter(espeakPITCH, 1);
    pitch = libToUserPitch(original);
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue eSpeakSynthesizer::synthesize(const std::string & text, yarp::sig::Sound & sound)
#else
bool eSpeakSynthesizer::synthesize(const std::string & text, yarp::sig::Sound & sound)
#endif
{
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------
