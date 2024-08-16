// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "Espeak.hpp"

#include <string>
#include <vector>

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

#include <espeak/speak_lib.h>

using namespace roboticslab;

namespace
{
    YARP_LOG_COMPONENT(ESPK, "rl.Espeak")

    void printError(const std::string & caller, espeak_ERROR code)
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
}

// -----------------------------------------------------------------------------

Espeak::Espeak()
{
    espeak_AUDIO_OUTPUT output = AUDIO_OUTPUT_PLAYBACK;
    int buflength = 500;
    char * path = nullptr;
    int options = 0;

    if (espeak_Initialize(output, buflength, path, options) == EE_INTERNAL_ERROR)
    {
        yCWarning(ESPK) << "espeak_Initialize() failed";
    }
}

// -----------------------------------------------------------------------------

Espeak::~Espeak()
{
    if (espeak_Terminate() != EE_OK)
    {
        yCWarning(ESPK, "espeak_Terminate() failed");
    }
}

// -----------------------------------------------------------------------------

bool Espeak::setLanguage(const std::string & language)
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

bool Espeak::say(const std::string & text)
{
    yCInfo(ESPK) << "say()" << text;

    for (int i = 0; i < text.length(); i++)
    {
        yCDebug(ESPK, "i[%d] char[%c] int[%d]", i, text[i], (int)text[i]);
    }

    unsigned int position = 0;
    espeak_POSITION_TYPE position_type = POS_CHARACTER;
    unsigned int end_position = 0;
    unsigned int flags = espeakCHARS_AUTO | espeakENDPAUSE;
    unsigned int * unique_identifier = nullptr;
    void * user_data = nullptr;

    auto ret = espeak_Synth(text.c_str(), text.length() + 1, position, position_type, end_position, flags, unique_identifier, user_data);

    if (ret != EE_OK)
    {
        printError("say()", ret);
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool Espeak::setSpeed(const std::int16_t speed)
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

bool Espeak::setPitch(const std::int16_t pitch)
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

std::int16_t Espeak::getSpeed()
{
    return espeak_GetParameter(espeakRATE, 1);
}

// -----------------------------------------------------------------------------

std::int16_t Espeak::getPitch()
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

// -----------------------------------------------------------------------------

bool Espeak::pause()
{
    yCWarning(ESPK) << "pause() not implemented";
    return false;
}


// -----------------------------------------------------------------------------

bool Espeak::checkSayDone()
{
    return espeak_IsPlaying() != 1;
}

// -----------------------------------------------------------------------------
