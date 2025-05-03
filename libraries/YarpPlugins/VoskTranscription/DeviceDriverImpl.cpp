// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "VoskTranscription.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool VoskTranscription::open(yarp::os::Searchable & config)
{
    if (!parseParams(config))
    {
        yCError(VOSK) << "Failed to parse parameters";
        return false;
    }

    rf.setDefaultContext("VoskTranscription");

    if (!m_modelPath.empty() && !setLanguage(m_modelPath))
    {
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
