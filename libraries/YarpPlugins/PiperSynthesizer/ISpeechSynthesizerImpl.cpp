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

    int ret = ::piper_synthesize_start(synth, text.c_str(), &options);

    if (ret != PIPER_OK)
    {
        yCError(PIPER) << "Failed to start synthesis:" << ret;
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    piper_audio_chunk chunk;

    while ((ret = ::piper_synthesize_next(synth, &chunk)) != PIPER_DONE)
    {
        if (ret != PIPER_OK)
        {
            yCError(PIPER) << "Failed to synthesize next chunk:" << ret;
            return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
        }

        yarp::sig::Sound subSound;
        subSound.resize(chunk.num_samples);
        subSound.setFrequency(chunk.sample_rate);

        for (auto i = 0; i < chunk.num_samples; ++i)
        {
            subSound.set(static_cast<std::int16_t>(chunk.samples[i] * 32767.0f), i);
        }

        sound.setFrequency(chunk.sample_rate);
        sound += subSound;

        yCDebug(PIPER) << "Synthesized chunk with" << chunk.num_samples << "samples at" << chunk.sample_rate << "Hz";
    }

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------
