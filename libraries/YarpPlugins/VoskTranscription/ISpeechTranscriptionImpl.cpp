// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "VoskTranscription.hpp"

#include <vector>

#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#include <json.h>

#include "LogComponent.hpp"

// ------------------- ISpeechTranscription Related ------------------------------------

yarp::dev::ReturnValue VoskTranscription::setLanguage(const std::string & language)
{
    yCInfo(VOSK) << "Setting language model:" << language;

    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("VoskTranscription");

    auto modelFullPath = rf.findFileByName(language);

    if (modelFullPath.empty())
    {
        yCError(VOSK) << "Model file not found";
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    yCInfo(VOSK) << "Loading model from:" << modelFullPath;

    model = vosk_model_new(modelFullPath.c_str());

    if (!model)
    {
        yCError(VOSK) << "Failed to load model";
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    modelName = language;

    if (recognizer)
    {
        vosk_recognizer_free(recognizer);
        recognizer = nullptr;
    }

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue VoskTranscription::getLanguage(std::string & language)
{
    if (modelName.empty())
    {
        yCError(VOSK) << "Language model not set";
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    language = modelName;

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue VoskTranscription::transcribe(const yarp::sig::Sound & sound, std::string & transcription, double & score)
{
    if (!model)
    {
        yCError(VOSK) << "Model is not loaded";
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    if (!recognizer || sampleRate != sound.getFrequency())
    {
        yCInfo(VOSK) << "Configuring new recognizer with sample rate:" << sound.getFrequency() << "Hz";

        if (recognizer)
        {
            vosk_recognizer_free(recognizer);
            recognizer = nullptr;
        }

        sampleRate = sound.getFrequency();
        recognizer = vosk_recognizer_new(model, sampleRate);

        if (recognizer == nullptr)
        {
            yCError(VOSK) << "Failed to create recognizer";
            return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
        }
    }

    std::vector<short> samples(sound.getSamples());

    for (auto i = 0; i < samples.size(); i++)
    {
        samples[i] = sound.get(i);
    }

    switch (vosk_recognizer_accept_waveform_s(recognizer, samples.data(), samples.size()))
    {
    case 1:
    {
        auto result = vosk_recognizer_result(recognizer);
        auto parsed = json::JSON::Load(result);
        transcription = parsed["text"].ToString();

        if (!transcription.empty())
        {
            yCDebug(VOSK) << "Result:" << transcription;
        }

        break;
    }
    case 0:
    {
        auto result = vosk_recognizer_partial_result(recognizer);
        auto parsed = json::JSON::Load(result);
        transcription = parsed["partial"].ToString();

        if (!transcription.empty())
        {
            yCDebug(VOSK) << "Partial result:" << transcription;
        }

        break;
    }
    case -1:
    default:
        yCError(VOSK) << "Failed to accept waveform";
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------
