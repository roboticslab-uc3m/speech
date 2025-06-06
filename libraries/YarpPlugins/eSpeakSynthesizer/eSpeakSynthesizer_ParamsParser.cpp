/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


// Generated by yarpDeviceParamParserGenerator (1.0)
// This is an automatically generated file. Please do not edit it.
// It will be re-generated if the cmake flag ALLOW_DEVICE_PARAM_PARSER_GERNERATION is ON.

// Generated on: Sun Apr 20 21:17:42 2025


#include "eSpeakSynthesizer_ParamsParser.h"
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

namespace {
    YARP_LOG_COMPONENT(eSpeakSynthesizerParamsCOMPONENT, "yarp.device.eSpeakSynthesizer")
}


eSpeakSynthesizer_ParamsParser::eSpeakSynthesizer_ParamsParser()
{
}


std::vector<std::string> eSpeakSynthesizer_ParamsParser::getListOfParams() const
{
    std::vector<std::string> params;
    params.push_back("voice");
    params.push_back("bufLength");
    return params;
}


bool      eSpeakSynthesizer_ParamsParser::parseParams(const yarp::os::Searchable & config)
{
    //Check for --help option
    if (config.check("help"))
    {
        yCInfo(eSpeakSynthesizerParamsCOMPONENT) << getDocumentationOfDeviceParams();
    }

    std::string config_string = config.toString();
    yarp::os::Property prop_check(config_string.c_str());
    //Parser of parameter voice
    {
        if (config.check("voice"))
        {
            m_voice = config.find("voice").asString();
            yCInfo(eSpeakSynthesizerParamsCOMPONENT) << "Parameter 'voice' using value:" << m_voice;
        }
        else
        {
            yCInfo(eSpeakSynthesizerParamsCOMPONENT) << "Parameter 'voice' using DEFAULT value:" << m_voice;
        }
        prop_check.unput("voice");
    }

    //Parser of parameter bufLength
    {
        if (config.check("bufLength"))
        {
            m_bufLength = config.find("bufLength").asInt64();
            yCInfo(eSpeakSynthesizerParamsCOMPONENT) << "Parameter 'bufLength' using value:" << m_bufLength;
        }
        else
        {
            yCInfo(eSpeakSynthesizerParamsCOMPONENT) << "Parameter 'bufLength' using DEFAULT value:" << m_bufLength;
        }
        prop_check.unput("bufLength");
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
                yCError(eSpeakSynthesizerParamsCOMPONENT) << "User asking for parameter: "<<it->name <<" which is unknown to this parser!";
                extra_params_found = true;
            }
            else
            {
                yCWarning(eSpeakSynthesizerParamsCOMPONENT) << "User asking for parameter: "<< it->name <<" which is unknown to this parser!";
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


std::string      eSpeakSynthesizer_ParamsParser::getDocumentationOfDeviceParams() const
{
    std::string doc;
    doc = doc + std::string("\n=============================================\n");
    doc = doc + std::string("This is the help for device: eSpeakSynthesizer\n");
    doc = doc + std::string("\n");
    doc = doc + std::string("This is the list of the parameters accepted by the device:\n");
    doc = doc + std::string("'voice': voice identifier\n");
    doc = doc + std::string("'bufLength': length of sound buffers\n");
    doc = doc + std::string("\n");
    doc = doc + std::string("Here are some examples of invocation command with yarpdev, with all params:\n");
    doc = doc + " yarpdev --device eSpeakSynthesizer --voice <optional_value> --bufLength 0\n";
    doc = doc + std::string("Using only mandatory params:\n");
    doc = doc + " yarpdev --device eSpeakSynthesizer\n";
    doc = doc + std::string("=============================================\n\n");    return doc;
}
