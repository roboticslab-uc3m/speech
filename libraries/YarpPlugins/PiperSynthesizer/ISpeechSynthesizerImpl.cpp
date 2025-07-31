// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "PiperSynthesizer.hpp"

#include <algorithm> // std::find_if
#include <cmath> // std::max
#include <cstdint>

#include <vector>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

// ------------------- ISpeechSynthesizer Related ------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::setLanguage(const std::string & language)
{
    if (synth)
    {
        const auto name = toLowerCase(language);

        auto it = std::find_if(storage.begin(), storage.end(), [&name](const auto & entry) {
            return entry.second.language == name || entry.second.code == name;
        });

        if (it != storage.end())
        {
            current_model = &it->second;
            yCInfo(PIPER) << "Setting language to:" << name;
            loadCurrentModel();
            return yarp::dev::ReturnValue::return_code::return_value_ok;
        }
        else
        {
            yCError(PIPER) << "Language not found:" << name;
            return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
        }
    }
    else
    {
        yCError(PIPER) << "Synthesizer not initialized";
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::getLanguage(std::string & language)
{
    if (synth)
    {
        language = current_model->language;
        return yarp::dev::ReturnValue::return_code::return_value_ok;
    }
    else
    {
        yCError(PIPER) << "Synthesizer not initialized";
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::setVoice(const std::string & voice_name)
{
    if (synth)
    {
        const auto name = toLowerCase(voice_name);

        auto it = std::find_if(storage.begin(), storage.end(), [&name](const auto & entry) {
            return entry.second.dataset == name;
        });

        if (it != storage.end())
        {
            current_model = &it->second;
            yCInfo(PIPER) << "Setting voice to:" << name;
            loadCurrentModel();
            return yarp::dev::ReturnValue::return_code::return_value_ok;
        }
        else
        {
            yCError(PIPER) << "Voice not found:" << name;
            return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
        }
    }
    else
    {
        yCError(PIPER) << "Synthesizer not initialized";
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::getVoice(std::string & voice_name)
{
    if (synth)
    {
        voice_name = current_model->dataset;
        return yarp::dev::ReturnValue::return_code::return_value_ok;
    }
    else
    {
        yCError(PIPER) << "Synthesizer not initialized";
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::setSpeed(double speed)
{
    if (synth)
    {
        speed = std::max(speed, 0.0); // ensure speed is non-negative
        options.length_scale = 1.0 / speed;
        yCInfo(PIPER) << "Setting speed to:" << speed;
        return yarp::dev::ReturnValue::return_code::return_value_ok;
    }
    else
    {
        yCError(PIPER) << "Synthesizer not initialized";
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::getSpeed(double & speed)
{
    if (synth)
    {
        speed = 1.0 / options.length_scale;
        return yarp::dev::ReturnValue::return_code::return_value_ok;
    }
    else
    {
        yCError(PIPER) << "Synthesizer not initialized";
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue PiperSynthesizer::setPitch(double pitch)
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
    if (!synth)
    {
        yCError(PIPER) << "Synthesizer not initialized";
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }

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
