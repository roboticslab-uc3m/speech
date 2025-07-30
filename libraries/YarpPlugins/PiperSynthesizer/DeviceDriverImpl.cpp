// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "PiperSynthesizer.hpp"

#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#include "LogComponent.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool PiperSynthesizer::open(yarp::os::Searchable & config)
{
    if (!parseParams(config))
    {
        yCError(PIPER) << "Failed to parse parameters";
        return false;
    }

    if (m_modelPath.empty())
    {
        yCError(PIPER) << "Model path is empty";
        return false;
    }

    if (m_modelConfigPath.empty())
    {
        m_modelConfigPath = m_modelPath + ".json";
    }

    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("PiperSynthesizer");

    auto modelFullPath = rf.findFileByName(m_modelPath);

    if (modelFullPath.empty())
    {
        yCError(PIPER) << "Model file not found:" << m_modelPath;
        return false;
    }

    yCInfo(PIPER) << "Loading model from:" << modelFullPath;

    auto modelConfigFullPath = rf.findFileByName(m_modelConfigPath);

    if (modelConfigFullPath.empty())
    {
        yCError(PIPER) << "Model config file not found:" << m_modelConfigPath;
        return false;
    }

    yCInfo(PIPER) << "Loading model config from:" << modelConfigFullPath;

    std::string eSpeakDataFullPath;

    if (!m_eSpeakDataPath.empty())
    {
        eSpeakDataFullPath = rf.findFileByName(m_eSpeakDataPath);

        if (eSpeakDataFullPath.empty())
        {
            yCError(PIPER) << "eSpeak data path not found:" << m_eSpeakDataPath;
            return false;
        }
    }
    else
    {
        eSpeakDataFullPath = std::string(_ESPEAKNG_DATA_DIR);
    }

    yCInfo(PIPER) << "Using eSpeak data path:" << eSpeakDataFullPath;

    synth = ::piper_create(modelFullPath.c_str(), modelConfigFullPath.c_str(), eSpeakDataFullPath.c_str());
    options = ::piper_default_synthesize_options(synth);

    options.speaker_id = m_speakerId;
    options.length_scale = m_lengthScale;
    options.noise_scale = m_noiseScale;
    options.noise_w_scale = m_noiseW;

    return true;
}

// -----------------------------------------------------------------------------

bool PiperSynthesizer::close()
{
    ::piper_free(synth);
    return true;
}

// -----------------------------------------------------------------------------
