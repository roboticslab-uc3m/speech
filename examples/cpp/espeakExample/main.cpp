// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup teo_examples_cpp
 * @defgroup espeakExample espeakExample
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/SystemClock.h>

#include <yarp/dev/PolyDriver.h>

#include <Speech_IDL.h>

int main(int argc, char **argv)
{
    yarp::os::Network yarp;

    if (!yarp::os::Network::checkNetwork())
    {
        yError() << "Please start a yarp name server first";
        return 1;
    }

    yarp::os::Property options {{"device", yarp::os::Value("Espeak")}};
    yarp::dev::PolyDriver dd;

    if (!dd.open(options))
    {
        yError() << "Device not available";
        return 1;
    }

    Speech_IDL * espeak;

    if (!dd.view(espeak))
    {
        yError() << "Unable to acquire espeak interface";
        return 1;
    }

    yInfo() << "Acquired espeak interface";

    espeak->setSpeed(150); // Values 80 to 450.
    espeak->setPitch(60); // 50 = normal

    yInfo() << "Using speed" << espeak->getSpeed();
    yInfo() << "Using pitch" << espeak->getPitch();

    espeak->say("Hello, my name is Teo. I want to follow you. Please, tell me. Ok, I will follow you. Ok, I will stop following you.");

    do
    {
        yarp::os::SystemClock::delaySystem(0.1);
    }
    while (!espeak->checkSayDone());

    return 0;
}
