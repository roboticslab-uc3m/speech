// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "VoskTranscription.hpp"

#include <vector>

#include <yarp/os/LogStream.h>

#include <json.h>

#include "LogComponent.hpp"

// ------------------- ISpeechTranscription Related ------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 11, 0)
yarp::dev::ReturnValue VoskTranscription::setLanguage(const std::string & language)
#else
bool VoskTranscription::setLanguage(const std::string & language)
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
yarp::dev::ReturnValue VoskTranscription::getLanguage(std::string & language)
#else
bool VoskTranscription::getLanguage(std::string & language)
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
yarp::dev::ReturnValue VoskTranscription::transcribe(const yarp::sig::Sound & sound, std::string & transcription, double & score)
#else
bool VoskTranscription::transcribe(const yarp::sig::Sound & sound, std::string & transcription, double & score)
#endif
{
    if (sampleRate != sound.getFrequency())
    {
        yCInfo(VOSK) << "Setting new sample rate:" << sound.getFrequency() << "Hz";

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
        yCDebug(VOSK) << "Result:" << transcription;
        break;
    }
    case 0:
    {
        auto result = vosk_recognizer_partial_result(recognizer);
        auto parsed = json::JSON::Load(result);
        auto partial = parsed["partial"].ToString();

        if (!partial.empty())
        {
            yCDebug(VOSK) << "Partial result:" << partial;
        }

        transcription = ""; // signalize an incomplete transcription
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
