// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "VoskTranscription.hpp"

#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#include "LogComponent.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool VoskTranscription::open(yarp::os::Searchable & config)
{
    if (!parseParams(config))
    {
        yCError(VOSK) << "Failed to parse parameters";
        return false;
    }

    if (m_modelPath.empty())
    {
        yCError(VOSK) << "Model path is empty";
        return false;
    }

    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("VoskSynthesizer");

    auto modelFullPath = rf.findFileByName(m_modelPath);

    if (modelFullPath.empty())
    {
        yCError(VOSK) << "Model file not found:" << m_modelPath;
        return false;
    }

    yCInfo(VOSK) << "Loading model from:" << modelFullPath;

    model = vosk_model_new(modelFullPath.c_str());

    if (model == nullptr)
    {
        yCError(VOSK) << "Failed to load model from:" << modelFullPath;
        return false;
    }

    if (m_sampleRate <= 0)
    {
        yCError(VOSK) << "Sample rate is not set or invalid";
        return false;
    }

    recognizer = vosk_recognizer_new(model, m_sampleRate);

    if (recognizer == nullptr)
    {
        yCError(VOSK) << "Failed to create recognizer";
        return false;
    }

    if (m_useGPU)
    {
        vosk_gpu_init();
    }

    return true;
}

// -----------------------------------------------------------------------------

bool VoskTranscription::close()
{
    if (recognizer)
    {
        vosk_recognizer_free(recognizer);
        recognizer = nullptr;
    }

    if (model)
    {
        vosk_model_free(model);
        model = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------------
