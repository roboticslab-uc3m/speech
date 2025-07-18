// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "PiperSynthesizer.hpp"

#include <cstdint>

#include <vector>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

// ------------------- ISpeechSynthesizer Related ------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::setLanguage(const std::string & language)
{
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::getLanguage(std::string & language)
{
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::setVoice(const std::string & voice_name)
{
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::getVoice(std::string & voice_name)
{
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::setSpeed(const double speed)
{
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::getSpeed(double & speed)
{
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::setPitch(const double pitch)
{
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::getPitch(double & pitch)
{
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::synthesize(const std::string & text, yarp::sig::Sound & sound)
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

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------
