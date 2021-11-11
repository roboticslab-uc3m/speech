// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>

#include "EspeakServer.hpp"

/**
 * @ingroup speech-programs
 * @defgroup espeakServer espeakServer
 * @brief Creates an instance of roboticslab::EspeakServer.
 */

int main(int argc, char * argv[])
{
    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("espeakServer");
    rf.setDefaultConfigFile("espeakServer.ini");
    rf.configure(argc, argv);

    roboticslab::EspeakServer mod;

    yInfo() << "espeakServer checking for yarp network...";

    if (!yarp::os::Network::checkNetwork())
    {
        yError() << "espeakServer found no yarp network (try running \"yarpserver &\")";
        return 1;
    }

    return mod.runModule(rf);
}
