// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "EspeakServer.hpp"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace roboticslab;

namespace
{
    YARP_LOG_COMPONENT(ESS, "rl.EspeakServer")
}

constexpr auto DEFAULT_VOICE = "mb-en1";
constexpr auto DEFAULT_PORT = "/tts";

bool EspeakServer::configure(yarp::os::ResourceFinder & rf)
{
    yCDebug(ESS) << "Config:" << rf.toString();

    auto language = rf.check("language", yarp::os::Value(DEFAULT_VOICE), "voice (e.g. mb-en1, mb-es1)").asString();

    if (!espeak.setLanguage(language))
    {
        yCError(ESS) << "Failed to set language to" << language;
        return false;
    }

    auto portName = rf.check("name", yarp::os::Value(DEFAULT_PORT), "server port name").asString();

    if (!port.open(portName + "/rpc:s"))
    {
        yCError(ESS) << "Failed to open port" << port.getName();
        return false;
    }

    return espeak.yarp().attachAsServer(port);
}
