/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


// Generated by yarpDeviceParamParserGenerator (1.0)
// This is an automatically generated file. Please do not edit it.
// It will be re-generated if the cmake flag ALLOW_DEVICE_PARAM_PARSER_GERNERATION is ON.

// Generated on: Sun Apr 27 19:48:31 2025


#include "PiperSynthesizer_ParamsParser.h"
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

namespace {
    YARP_LOG_COMPONENT(PiperSynthesizerParamsCOMPONENT, "yarp.device.PiperSynthesizer")
}


PiperSynthesizer_ParamsParser::PiperSynthesizer_ParamsParser()
{
}


std::vector<std::string> PiperSynthesizer_ParamsParser::getListOfParams() const
{
    std::vector<std::string> params;
    params.push_back("modelPath");
    params.push_back("modelConfigPath");
    params.push_back("eSpeakDataPath");
    params.push_back("tashkeelModelPath");
    params.push_back("speakerId");
    params.push_back("noiseScale");
    params.push_back("lengthScale");
    params.push_back("noiseW");
    params.push_back("sentenceSilenceSeconds");
    params.push_back("useCuda");
    return params;
}


bool      PiperSynthesizer_ParamsParser::parseParams(const yarp::os::Searchable & config)
{
    //Check for --help option
    if (config.check("help"))
    {
        yCInfo(PiperSynthesizerParamsCOMPONENT) << getDocumentationOfDeviceParams();
    }

    std::string config_string = config.toString();
    yarp::os::Property prop_check(config_string.c_str());
    //Parser of parameter modelPath
    {
        if (config.check("modelPath"))
        {
            m_modelPath = config.find("modelPath").asString();
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'modelPath' using value:" << m_modelPath;
        }
        else
        {
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'modelPath' using DEFAULT value:" << m_modelPath;
        }
        prop_check.unput("modelPath");
    }

    //Parser of parameter modelConfigPath
    {
        if (config.check("modelConfigPath"))
        {
            m_modelConfigPath = config.find("modelConfigPath").asString();
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'modelConfigPath' using value:" << m_modelConfigPath;
        }
        else
        {
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'modelConfigPath' using DEFAULT value:" << m_modelConfigPath;
        }
        prop_check.unput("modelConfigPath");
    }

    //Parser of parameter eSpeakDataPath
    {
        if (config.check("eSpeakDataPath"))
        {
            m_eSpeakDataPath = config.find("eSpeakDataPath").asString();
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'eSpeakDataPath' using value:" << m_eSpeakDataPath;
        }
        else
        {
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'eSpeakDataPath' using DEFAULT value:" << m_eSpeakDataPath;
        }
        prop_check.unput("eSpeakDataPath");
    }

    //Parser of parameter tashkeelModelPath
    {
        if (config.check("tashkeelModelPath"))
        {
            m_tashkeelModelPath = config.find("tashkeelModelPath").asString();
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'tashkeelModelPath' using value:" << m_tashkeelModelPath;
        }
        else
        {
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'tashkeelModelPath' using DEFAULT value:" << m_tashkeelModelPath;
        }
        prop_check.unput("tashkeelModelPath");
    }

    //Parser of parameter speakerId
    {
        if (config.check("speakerId"))
        {
            m_speakerId = config.find("speakerId").asInt64();
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'speakerId' using value:" << m_speakerId;
        }
        else
        {
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'speakerId' using DEFAULT value:" << m_speakerId;
        }
        prop_check.unput("speakerId");
    }

    //Parser of parameter noiseScale
    {
        if (config.check("noiseScale"))
        {
            m_noiseScale = config.find("noiseScale").asFloat32();
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'noiseScale' using value:" << m_noiseScale;
        }
        else
        {
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'noiseScale' using DEFAULT value:" << m_noiseScale;
        }
        prop_check.unput("noiseScale");
    }

    //Parser of parameter lengthScale
    {
        if (config.check("lengthScale"))
        {
            m_lengthScale = config.find("lengthScale").asFloat32();
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'lengthScale' using value:" << m_lengthScale;
        }
        else
        {
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'lengthScale' using DEFAULT value:" << m_lengthScale;
        }
        prop_check.unput("lengthScale");
    }

    //Parser of parameter noiseW
    {
        if (config.check("noiseW"))
        {
            m_noiseW = config.find("noiseW").asFloat32();
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'noiseW' using value:" << m_noiseW;
        }
        else
        {
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'noiseW' using DEFAULT value:" << m_noiseW;
        }
        prop_check.unput("noiseW");
    }

    //Parser of parameter sentenceSilenceSeconds
    {
        if (config.check("sentenceSilenceSeconds"))
        {
            m_sentenceSilenceSeconds = config.find("sentenceSilenceSeconds").asFloat32();
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'sentenceSilenceSeconds' using value:" << m_sentenceSilenceSeconds;
        }
        else
        {
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'sentenceSilenceSeconds' using DEFAULT value:" << m_sentenceSilenceSeconds;
        }
        prop_check.unput("sentenceSilenceSeconds");
    }

    //Parser of parameter useCuda
    {
        if (config.check("useCuda"))
        {
            m_useCuda = config.find("useCuda").asBool();
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'useCuda' using value:" << m_useCuda;
        }
        else
        {
            yCInfo(PiperSynthesizerParamsCOMPONENT) << "Parameter 'useCuda' using DEFAULT value:" << m_useCuda;
        }
        prop_check.unput("useCuda");
    }

    /*
    //This code check if the user set some parameter which are not check by the parser
    //If the parser is set in strict mode, this will generate an error
    if (prop_check.size() > 0)
    {
        bool extra_params_found = false;
        for (auto it=prop_check.begin(); it!=prop_check.end(); it++)
        {
            if (m_parser_is_strict)
            {
                yCError(PiperSynthesizerParamsCOMPONENT) << "User asking for parameter: "<<it->name <<" which is unknown to this parser!";
                extra_params_found = true;
            }
            else
            {
                yCWarning(PiperSynthesizerParamsCOMPONENT) << "User asking for parameter: "<< it->name <<" which is unknown to this parser!";
            }
        }

       if (m_parser_is_strict && extra_params_found)
       {
           return false;
       }
    }
    */
    return true;
}


std::string      PiperSynthesizer_ParamsParser::getDocumentationOfDeviceParams() const
{
    std::string doc;
    doc = doc + std::string("\n=============================================\n");
    doc = doc + std::string("This is the help for device: PiperSynthesizer\n");
    doc = doc + std::string("\n");
    doc = doc + std::string("This is the list of the parameters accepted by the device:\n");
    doc = doc + std::string("'modelPath': path to .onnx voice file\n");
    doc = doc + std::string("'modelConfigPath': path to JSON voice config file\n");
    doc = doc + std::string("'eSpeakDataPath': path to espeak-ng data directory\n");
    doc = doc + std::string("'tashkeelModelPath': path to libtashkeel ort model\n");
    doc = doc + std::string("'speakerId': id of speaker\n");
    doc = doc + std::string("'noiseScale': generator noise\n");
    doc = doc + std::string("'lengthScale': phoneme length\n");
    doc = doc + std::string("'noiseW': phoneme width noise\n");
    doc = doc + std::string("'sentenceSilenceSeconds': silence after each sentence\n");
    doc = doc + std::string("'useCuda': use CUDA execution provider\n");
    doc = doc + std::string("\n");
    doc = doc + std::string("Here are some examples of invocation command with yarpdev, with all params:\n");
    doc = doc + " yarpdev --device PiperSynthesizer --modelPath <optional_value> --modelConfigPath <optional_value> --eSpeakDataPath <optional_value> --tashkeelModelPath <optional_value> --speakerId 0 --noiseScale 0.667 --lengthScale 1 --noiseW 0.8 --sentenceSilenceSeconds 0.2 --useCuda 0\n";
    doc = doc + std::string("Using only mandatory params:\n");
    doc = doc + " yarpdev --device PiperSynthesizer\n";
    doc = doc + std::string("=============================================\n\n");    return doc;
}
