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

    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("PiperSynthesizer");

    if (!m_eSpeakDataDir.empty())
    {
        eSpeakDataFullPath = rf.findFileByName(m_eSpeakDataDir);

        if (eSpeakDataFullPath.empty())
        {
            yCError(PIPER) << "eSpeak data path not found:" << m_eSpeakDataDir;
            return false;
        }
    }
    else
    {
        eSpeakDataFullPath = _ESPEAKNG_DATA_DIR;
    }

    yCInfo(PIPER) << "Using eSpeak data path:" << eSpeakDataFullPath;

    auto baseFullPath = rf.findFileByName("PiperSynthesizer.ini");

    if (baseFullPath.empty())
    {
        yCError(PIPER) << "Base model path not found";
        return false;
    }

    const auto base = std::filesystem::path(baseFullPath).parent_path();

    yCDebug(PIPER) << "Searching for models in:" << base.string();

    if (!inspectModels(base))
    {
        yCError(PIPER) << "Failed to inspect models";
        return false;
    }

    if (!m_model.empty())
    {
        auto it = storage.find(m_model);

        if (it == storage.end())
        {
            yCError(PIPER) << "Model not found:" << m_model;
            return false;
        }

        current_model = &it->second;
        loadCurrentModel();
        yCDebug(PIPER) << "Loaded model:" << m_model;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool PiperSynthesizer::close()
{
    if (synth)
    {
        ::piper_free(synth);
        synth = nullptr;
    }

    return true;
}

// -----------------------------------------------------------------------------
