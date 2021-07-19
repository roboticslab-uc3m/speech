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

// -----------------------------------------------------------------------------

Espeak::Espeak()
{
    path = nullptr;
    buflength = 500;
    options = 0;
    position = 0;
    position_type = POS_CHARACTER;
    end_position = 0;
    flags = espeakCHARS_AUTO | espeakENDPAUSE;
    unique_identifier = nullptr;
    user_data = nullptr;
    output = AUDIO_OUTPUT_PLAYBACK;
}

// -----------------------------------------------------------------------------

void Espeak::printError(espeak_ERROR code)
{
    if (code == EE_INTERNAL_ERROR)
        yCError(ESPK) << "EE_INTERNAL_ERROR";
    if (code == EE_BUFFER_FULL)
        yCError(ESPK) << "EE_BUFFER_FULL";
    if (code == EE_NOT_FOUND)
        yCError(ESPK) << "EE_NOT_FOUND";
}

// -----------------------------------------------------------------------------

bool Espeak::setLanguage(const std::string& language)
{
    espeak_ERROR ret = espeak_SetVoiceByName(language.c_str());
    if ( ret != EE_OK)
    {
        yCError(ESPK) << "setLanguage():" << language;
        printError(ret);
        return false;
    }
    yCInfo(ESPK) << "setLanguage()" << language;
    return true;
}

// -----------------------------------------------------------------------------

std::vector<std::string> Espeak::getSupportedLang()
{
    std::vector<std::string> supportedLang;

    //-- Does not show mbrola
    /* const espeak_VOICE** list = espeak_ListVoices(NULL);
    int i = 0;
    while (list[i] != NULL)
    {
        CD_DEBUG("%s\n",list[i]->languages);
        supportedLang.push_back(list[i]->languages);
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
        supportedLang.push_back(list[i]->languages);
        i++;
    }*/

    //-- Hard-coded for now
    std::string mb_en1("mb-en1");
    supportedLang.push_back(mb_en1);
    std::string mb_es1("mb-es1");
    supportedLang.push_back(mb_es1);

    return supportedLang;
}

// -----------------------------------------------------------------------------
bool Espeak::say(const std::string& text)
{
    int s = text.length();
    const char* c = text.c_str();
    for(int i=0;i<s+1;i++)
    {
        std::printf("i[%d] char[%c] int[%d]\n",i,c[i],(int)c[i]);
    }
    espeak_ERROR ret = espeak_Synth( text.c_str(), text.length()+1, position, position_type, end_position, flags, unique_identifier, user_data );
    if ( ret != EE_OK)
    {
        yCError(ESPK) << "say():" << text;
        printError(ret);
        return false;
    }
    espeak_Synchronize();  //-- Just for blocking

    yCInfo(ESPK) << "say():" << text;
    return true;
}

// -----------------------------------------------------------------------------

bool Espeak::setSpeed(const int16_t speed)
{
    espeak_ERROR ret = espeak_SetParameter(espeakRATE, speed, 0);
    if ( ret != EE_OK)
    {
        yCError(ESPK) << "setSpeed():" << speed;
        printError(ret);
        return false;
    }
    yCInfo(ESPK) << "setSpeed():" << speed;
    return true;
}

// -----------------------------------------------------------------------------

 bool Espeak::setPitch(const int16_t pitch)
 {
    espeak_ERROR ret = espeak_SetParameter(espeakPITCH, pitch, 0);
    if ( ret != EE_OK)
    {
        yCError(ESPK) << "setPitch():" << pitch;
        printError(ret);
        return false;
    }
    yCInfo(ESPK) << "setPitch():" << pitch;
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
    //-- Wolud need to store previous say().
    yCWarning(ESPK) << "play() not implemented, use say()";
    return false;
}

 // -----------------------------------------------------------------------------

bool Espeak::stop()
{
    espeak_ERROR ret = espeak_Cancel();
    if ( ret != EE_OK)
    {
        yCError(ESPK) << "stop()";
        printError(ret);
        return false;
    }
    yCInfo(ESPK) << "stop()";
    return true;
}


// ------------------- DeviceDriver Related ------------------------------------

bool Espeak::open(yarp::os::Searchable& config)
{
    std::string name = config.check("name",yarp::os::Value(DEFAULT_NAME),"port /name (auto append of /rpc:s)").asString();
    std::string voice = config.check("voice",yarp::os::Value(DEFAULT_VOICE),"voice").asString();

    yCDebug(ESPK, "--name: %s [%s]",name.c_str(),DEFAULT_NAME);
    yCDebug(ESPK, "--voice: %s [%s]",voice.c_str(),DEFAULT_VOICE);

    int ret = espeak_Initialize(output, buflength, path, options);
    if(ret == EE_INTERNAL_ERROR)
        return false;

    if( ! setLanguage(voice) )
        return false;

    name += "/rpc:s";

    this->yarp().attachAsServer(rpcPort);

    if( ! rpcPort.open(name.c_str()) )
    {
        yCError(ESPK) << "Cannot open port" << name;
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool Espeak::close()
{
    return true;
}

// -----------------------------------------------------------------------------
