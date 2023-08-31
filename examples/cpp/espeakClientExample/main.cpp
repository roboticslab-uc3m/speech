// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup speech-examples
 * @defgroup espeakClientExample espeakClientExample
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/SystemClock.h>

#include <SpeechSynthesis.h>

constexpr auto DEFAULT_REMOTE_PORT = "/tts";

int main(int argc, char * argv[])
{
    yarp::os::Property config;
    config.fromCommand(argc, argv);

    auto remote = config.check("remote", yarp::os::Value(DEFAULT_REMOTE_PORT), "remote server port").asString();

    yarp::os::Network yarp;

    if (!yarp::os::Network::checkNetwork())
    {
        yError() << "Please start a yarp name server first";
        return 1;
    }

    yarp::os::RpcClient client;

    if (!client.open("/espeakClientExample/rpc:c"))
    {
        yError() << "Unable to open client port" << client.getName();
        return 1;
    }

    if (!yarp::os::Network::connect(client.getName(), remote + "/rpc:s"))
    {
        yError() << "Unable to connect to remote server port" << remote;
        return 1;
    }

    roboticslab::SpeechSynthesis tts;
    tts.yarp().attachAsClient(client);

    yInfo() << "Connected to remote Espeak server";

    tts.setLanguage("mb-en1");
    tts.setSpeed(150); // Values 80 to 450.
    tts.setPitch(60); // 50 = normal

    yInfo() << "Using speed" << tts.getSpeed();
    yInfo() << "Using pitch" << tts.getPitch();

    tts.say("Hello, my name is Teo. I want to follow you. Please, tell me. Ok, I will follow you. Ok, I will stop following you.");

    do
    {
        yarp::os::SystemClock::delaySystem(0.1);
    }
    while (!tts.checkSayDone());

    return 0;
}
