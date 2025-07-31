// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup speech-examples-cpp
 * @defgroup espeakExample espeakExample
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/SystemClock.h>

#include <Espeak.hpp>

int main(int argc, char * argv[])
{
    roboticslab::Espeak espeak;

    espeak.setLanguage("mb-en1");
    espeak.setSpeed(150); // Values 80 to 450.
    espeak.setPitch(60); // 50 = normal

    yInfo() << "Using speed" << espeak.getSpeed();
    yInfo() << "Using pitch" << espeak.getPitch();

    espeak.say("Hello, my name is Teo. I want to follow you. Please, tell me. Ok, I will follow you. Ok, I will stop following you.");

    do
    {
        yarp::os::SystemClock::delaySystem(0.1);
    }
    while (!espeak.checkSayDone());

    return 0;
}
