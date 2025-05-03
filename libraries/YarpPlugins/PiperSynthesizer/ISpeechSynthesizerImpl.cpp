// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "PiperSynthesizer.hpp"

#include <cstdint>

#include <vector>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

// ------------------- ISpeechSynthesizer Related ------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue PiperSynthesizer::setLanguage(const std::string & language)
#else
bool PiperSynthesizer::setLanguage(const std::string & language)
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
yarp::dev::ReturnValue PiperSynthesizer::getLanguage(std::string & language)
#else
bool PiperSynthesizer::getLanguage(std::string & language)
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
yarp::dev::ReturnValue PiperSynthesizer::setVoice(const std::string & voice_name)
#else
bool PiperSynthesizer::setVoice(const std::string & voice_name)
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
yarp::dev::ReturnValue PiperSynthesizer::getVoice(std::string & voice_name)
#else
bool PiperSynthesizer::getVoice(std::string & voice_name)
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
yarp::dev::ReturnValue PiperSynthesizer::setSpeed(const double speed)
#else
bool PiperSynthesizer::setSpeed(const double speed)
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
yarp::dev::ReturnValue PiperSynthesizer::getSpeed(double & speed)
#else
bool PiperSynthesizer::getSpeed(double & speed)
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
yarp::dev::ReturnValue PiperSynthesizer::setPitch(const double pitch)
#else
bool PiperSynthesizer::setPitch(const double pitch)
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
yarp::dev::ReturnValue PiperSynthesizer::getPitch(double & pitch)
#else
bool PiperSynthesizer::getPitch(double & pitch)
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
yarp::dev::ReturnValue PiperSynthesizer::synthesize(const std::string & text, yarp::sig::Sound & sound)
#else
bool PiperSynthesizer::synthesize(const std::string & text, yarp::sig::Sound & sound)
#endif
{
    yCInfo(PIPER) << "Synthesizing:" << text;

    piper::SynthesisResult result;
    std::vector<std::int16_t> audioBuffer;

    sound.setFrequency(voice.synthesisConfig.sampleRate);

    piper::textToAudio(piperConfig, voice, text, audioBuffer, result, [&audioBuffer, &sound]()
    {
        sound.resize(audioBuffer.size());

        for (int i = 0; i < audioBuffer.size(); i++)
        {
            sound.set(audioBuffer[i], i);
        }
    });

    yCDebug(PIPER, "Real-time factor: %f (infer=%f sec, audio=%f sec)", result.realTimeFactor, result.inferSeconds, result.audioSeconds);

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------
