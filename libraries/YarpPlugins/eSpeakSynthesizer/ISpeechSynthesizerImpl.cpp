// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "eSpeakSynthesizer.hpp"

#include <algorithm> // std::clamp
#include <functional> // std::function

#include <yarp/os/LogStream.h>

#include <espeak/speak_lib.h>

#include "LogComponent.hpp"

namespace
{
    constexpr auto MAX_SPEED_RATE = espeakRATE_MAXIMUM / static_cast<double>(espeakRATE_NORMAL);
    constexpr auto MIN_SPEED_RATE = espeakRATE_MINIMUM / static_cast<double>(espeakRATE_NORMAL);

    inline int userToLibSpeed(double userSpeed)
    {
        return static_cast<int>(userSpeed * espeakRATE_NORMAL);
    }

    inline double libToUserSpeed(int libSpeed)
    {
        return static_cast<double>(libSpeed / espeakRATE_NORMAL);
    }

    inline int userToLibPitch(double userPitch)
    {
        return static_cast<int>(userPitch * 100.0);
    }

    inline double libToUserPitch(int libPitch)
    {
        return static_cast<double>(libPitch) / 100.0;
    }

    // https://caiorss.github.io/C-Cpp-Notes/passing-lambda.html

    using SynthCallback = std::function<t_espeak_callback>;

    SynthCallback & getCallback()
    {
        // Global variable, its lifetimes corresponds to the program lifetime.
        static SynthCallback callback;
        return callback;
    };

    void setCallback(SynthCallback func)
    {
        auto & callback = getCallback();
        callback = func;
    }

    int callbackAdapter(short * wav, int numSamples, espeak_EVENT * events)
    {
        return getCallback()(wav, numSamples, events);
    }

    void wrapperToSetSynthCallback(SynthCallback func)
    {
        setCallback(func);
        espeak_SetSynthCallback(&callbackAdapter);
    }
}

// ------------------- ISpeechSynthesizer Related ------------------------------------

yarp::dev::ReturnValue eSpeakSynthesizer::setLanguage(const std::string & language)
{
    yCInfo(ESS) << "Setting language to:" << language;
    espeak_VOICE voice_spec = {};
    voice_spec.languages = language.c_str();

    if (espeak_SetVoiceByProperties(&voice_spec) == EE_OK)
    {
        isVoiceSet = true;
        return yarp::dev::ReturnValue::return_code::return_value_ok;
    }
    else
    {
        yCError(ESS) << "Failed to set language:" << language;
        return yarp::dev::ReturnValue::return_code::return_value_error_generic;
    }
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue eSpeakSynthesizer::getLanguage(std::string & language)
{
    if (isVoiceSet)
    {
        const auto * voice = espeak_GetCurrentVoice();
        language = voice->languages;
        return yarp::dev::ReturnValue::return_code::return_value_ok;
    }
    else
    {
        yCError(ESS) << "No voice set";
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue eSpeakSynthesizer::setVoice(const std::string & voice_name)
{
    yCInfo(ESS) << "Setting voice to:" << voice_name;

    if (espeak_SetVoiceByName(voice_name.c_str()) == EE_OK)
    {
        isVoiceSet = true;
        return yarp::dev::ReturnValue::return_code::return_value_ok;
    }
    else
    {
        yCError(ESS) << "Failed to set voice:" << voice_name;
        return yarp::dev::ReturnValue::return_code::return_value_error_generic;
    }
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue eSpeakSynthesizer::getVoice(std::string & voice_name)
{
    if (isVoiceSet)
    {
        const auto * voice = espeak_GetCurrentVoice();
        voice_name = voice->name;
        return yarp::dev::ReturnValue::return_code::return_value_ok;
    }
    else
    {
        yCError(ESS) << "No voice set";
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue eSpeakSynthesizer::setSpeed(double speed)
{
    auto clamped = std::clamp(speed, MIN_SPEED_RATE, MAX_SPEED_RATE);
    yCInfo(ESS) << "Setting speed to:" << clamped;

    return espeak_SetParameter(espeakRATE, userToLibSpeed(clamped), 0) == EE_OK
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_generic;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue eSpeakSynthesizer::getSpeed(double & speed)
{
    auto original = espeak_GetParameter(espeakRATE, 1);
    speed = libToUserSpeed(original);
    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue eSpeakSynthesizer::setPitch(double pitch)
{
    auto clamped = std::clamp(pitch, 0.0, 1.0);
    yCInfo(ESS) << "Setting pitch to:" << clamped;

    return espeak_SetParameter(espeakPITCH, userToLibPitch(clamped), 0) == EE_OK
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_generic;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue eSpeakSynthesizer::getPitch(double & pitch)
{
    auto original = espeak_GetParameter(espeakPITCH, 1);
    pitch = libToUserPitch(original);
    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue eSpeakSynthesizer::synthesize(const std::string & text, yarp::sig::Sound & sound)
{
    yCInfo(ESS) << "Synthesizing:" << text;

    // espeak_Initialize() returs 22050 Hz, but it's misleading
    sound.setFrequency(16000);

    auto callback = [&sound](short * wav, int numSamples, espeak_EVENT * events)
    {
        yarp::sig::Sound chunk;
        chunk.setFrequency(sound.getFrequency());
        chunk.resize(numSamples);

        for (int i = 0; i < numSamples; i++)
        {
            chunk.set(wav[i], i);
        }

        sound += chunk;
        return 0;
    };

    wrapperToSetSynthCallback(callback);

    if (espeak_Synth(text.c_str(), 0, 0, POS_CHARACTER, 0, espeakCHARS_AUTO | espeakENDPAUSE, nullptr, nullptr) != EE_OK)
    {
        yCError(ESS) << "Failed to synthesize text";
        return yarp::dev::ReturnValue::return_code::return_value_error_generic;
    }

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------
