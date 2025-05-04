// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "VoskTranscription.hpp"

#include <vector>

#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#include <json.h>

#include "LogComponent.hpp"

// ------------------- ISpeechTranscription Related ------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue VoskTranscription::setLanguage(const std::string & language)
#else
bool VoskTranscription::setLanguage(const std::string & language)
#endif
{
    yCInfo(VOSK) << "Setting language model:" << language;

    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("VoskTranscription");

    auto modelFullPath = rf.findFileByName(language);

    if (modelFullPath.empty())
    {
        yCError(VOSK) << "Model file not found";
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    yCInfo(VOSK) << "Loading model from:" << modelFullPath;

    model = vosk_model_new(modelFullPath.c_str());

    if (!model)
    {
        yCError(VOSK) << "Failed to load model";
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    modelName = language;

    if (recognizer)
    {
        vosk_recognizer_free(recognizer);
        recognizer = nullptr;
    }

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue VoskTranscription::getLanguage(std::string & language)
#else
bool VoskTranscription::getLanguage(std::string & language)
#endif
{
    if (modelName.empty())
    {
        yCError(VOSK) << "Language model not set";
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    language = modelName;

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue VoskTranscription::transcribe(const yarp::sig::Sound & sound, std::string & transcription, double & score)
#else
bool VoskTranscription::transcribe(const yarp::sig::Sound & sound, std::string & transcription, double & score)
#endif
{
    if (!model)
    {
        yCError(VOSK) << "Model is not loaded";
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
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
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
            return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
            return false;
#endif
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
#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------
