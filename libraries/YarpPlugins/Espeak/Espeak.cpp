// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "Espeak.hpp"

#include <cstdio>

#include <string>
#include <vector>

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

using namespace roboticslab;

namespace
{
    YARP_LOG_COMPONENT(ESPK, "rl.Espeak")
}

constexpr auto DEFAULT_NAME = "/espeak";
constexpr auto DEFAULT_VOICE = "mb-en1";

// -----------------------------------------------------------------------------

void Espeak::printError(const std::string & caller, espeak_ERROR code) const
{
    switch (code)
    {
    case EE_OK:
        yCError(ESPK) << caller << "EE_OK";
        break;
    case EE_BUFFER_FULL:
        yCError(ESPK) << caller << "EE_BUFFER_FULL";
        break;
    case EE_NOT_FOUND:
        yCError(ESPK) << caller << "EE_NOT_FOUND";
        break;
    }
}

// -----------------------------------------------------------------------------

bool Espeak::setLanguage(const std::string& language)
{
    yCInfo(ESPK) << "setLanguage()" << language;
    auto ret = espeak_SetVoiceByName(language.c_str());

    if (ret != EE_OK)
    {
        printError("setLanguage()", ret);
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

std::vector<std::string> Espeak::getSupportedLangs()
{
    std::vector<std::string> supportedLangs;

    //-- Does not show mbrola
    /* const espeak_VOICE** list = espeak_ListVoices(NULL);
    int i = 0;
    while (list[i] != NULL)
    {
        CD_DEBUG("%s\n",list[i]->languages);
        supportedLangs.push_back(list[i]->languages);
        i++;
    }*/

    //-- Not working, attempt to show mbrola
    /*espeak_VOICE test;
    test.languages = "mb";
    const espeak_VOICE** list = espeak_ListVoices(&test);
    int i = 0;
    while (list[i] != NULL)
    {
        CD_DEBUG("%s\n",list[i]->languages);
        supportedLangs.push_back(list[i]->languages);
        i++;
    }*/

    //-- Hard-coded for now
    supportedLangs.emplace_back("mb-en1");
    supportedLangs.emplace_back("mb-es1");

    return supportedLangs;
}

// -----------------------------------------------------------------------------

bool Espeak::say(const std::string& text)
{
    yCInfo(ESPK) << "say()" << text;
    const char * c = text.c_str();

    for (int i = 0; i < text.length() + 1; i++)
    {
        std::printf("i[%d] char[%c] int[%d]\n", i, c[i], (int)c[i]);
    }

    auto ret = espeak_Synth(text.c_str(), text.length() + 1, position, position_type, end_position, flags, unique_identifier, user_data);

    if (ret != EE_OK)
    {
        printError("say()", ret);
        return false;
    }

    espeak_Synchronize(); //-- Just for blocking

    yCInfo(ESPK) << "say() completed";
    return true;
}

// -----------------------------------------------------------------------------

bool Espeak::setSpeed(const int16_t speed)
{
    yCInfo(ESPK) << "setSpeed()" << speed;

    auto ret = espeak_SetParameter(espeakRATE, speed, 0);

    if (ret != EE_OK)
    {
        printError("setSpeed()", ret);
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool Espeak::setPitch(const int16_t pitch)
{
    yCInfo(ESPK) << "setPitch()" << pitch;
    auto ret = espeak_SetParameter(espeakPITCH, pitch, 0);

    if (ret != EE_OK)
    {
        printError("setPitch()", ret);
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

int16_t Espeak::getSpeed()
{
    return espeak_GetParameter(espeakRATE, 1);
}

// -----------------------------------------------------------------------------

int16_t Espeak::getPitch()
{
    return espeak_GetParameter(espeakPITCH, 1);
}

// -----------------------------------------------------------------------------

bool Espeak::play()
{
    //-- Would need to store previous say().
    yCWarning(ESPK) << "play() not implemented, use say()";
    return false;
}

// -----------------------------------------------------------------------------

bool Espeak::stop()
{
    yCInfo(ESPK) << "stop()";
    auto ret = espeak_Cancel();

    if (ret != EE_OK)
    {
        printError("stop()", ret);
        return false;
    }

    return true;
}

// ------------------- DeviceDriver Related ------------------------------------

bool Espeak::open(yarp::os::Searchable& config)
{
    auto name = config.check("name", yarp::os::Value(DEFAULT_NAME), "port /name (auto append of /rpc:s)").asString();
    auto voice = config.check("voice", yarp::os::Value(DEFAULT_VOICE), "voice").asString();

    yCDebug(ESPK, "--name: %s [%s]", name.c_str(), DEFAULT_NAME);
    yCDebug(ESPK, "--voice: %s [%s]", voice.c_str(), DEFAULT_VOICE);

    if (espeak_Initialize(output, buflength, path, options) == EE_INTERNAL_ERROR)
    {
        yCError(ESPK) << "espeak_Initialize()";
        return false;
    }

    if (!setLanguage(voice))
    {
        return false;
    }

    yarp().attachAsServer(rpcPort);

    if (!rpcPort.open(name + "/rpc:s"))
    {
        yCError(ESPK) << "Cannot open port" << rpcPort.getName();
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool Espeak::close()
{
    return espeak_Terminate() == EE_OK;
}

// -----------------------------------------------------------------------------
