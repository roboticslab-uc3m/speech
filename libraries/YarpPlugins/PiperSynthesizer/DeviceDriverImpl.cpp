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

    yCInfo(PIPER) << "Piper version:" << piper::getVersion();

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

    auto speakerId = std::optional(static_cast<std::int64_t>(m_speakerId));
    piper::loadVoice(piperConfig, modelFullPath, modelConfigFullPath, voice, speakerId, m_useCuda);

    if (voice.phonemizeConfig.phonemeType == piper::eSpeakPhonemes)
    {
        if (!m_eSpeakDataPath.empty())
        {
            auto eSpeakDataFullPath = rf.findFileByName(m_eSpeakDataPath);

            if (eSpeakDataFullPath.empty())
            {
                yCError(PIPER) << "eSpeak data path not found:" << m_eSpeakDataPath;
                return false;
            }

            piperConfig.eSpeakDataPath = eSpeakDataFullPath;
        }
        else
        {
            piperConfig.eSpeakDataPath = std::string(_PIPER_DATA_DIR) + "/espeak-ng-data";
        }

        yCInfo(PIPER) << "Using eSpeak data path:" << piperConfig.eSpeakDataPath;
    }
    else
    {
        piperConfig.useESpeak = false;
    }

    if (voice.phonemizeConfig.eSpeak.voice == "ar")
    {
        piperConfig.useTashkeel = true;

        if (!m_tashkeelModelPath.empty())
        {
            auto tashkeelModelFullPath = rf.findFileByName(m_tashkeelModelPath);

            if (tashkeelModelFullPath.empty())
            {
                yCError(PIPER) << "Tashkeel model path not found:" << m_tashkeelModelPath;
                return false;
            }

            piperConfig.tashkeelModelPath = tashkeelModelFullPath;
        }
        else
        {
            piperConfig.tashkeelModelPath = std::string(_PIPER_DATA_DIR) + "/libtashkeel_model.ort";
        }

        yCInfo(PIPER) << "Using Tashkeel model path:" << piperConfig.tashkeelModelPath.value();
    }

    piper::initialize(piperConfig);

    voice.synthesisConfig.noiseScale = m_noiseScale;
    voice.synthesisConfig.lengthScale = m_lengthScale;
    voice.synthesisConfig.noiseW = m_noiseW;
    voice.synthesisConfig.sentenceSilenceSeconds = m_sentenceSilenceSeconds;

    return true;
}

// -----------------------------------------------------------------------------

bool PiperSynthesizer::close()
{
    piper::terminate(piperConfig);
    return true;
}

// -----------------------------------------------------------------------------
