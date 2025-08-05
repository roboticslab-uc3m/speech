// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup speech-examples-cpp
 * @defgroup speechSynthesizerClientExample speechSynthesizerClientExample
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/SystemClock.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ISpeechSynthesizer.h>

constexpr auto DEFAULT_REMOTE_PORT = "/speechSynthesizer_nws";

int main(int argc, char * argv[])
{
    yarp::os::Network yarp;

    if (!yarp::os::Network::checkNetwork())
    {
        yError() << "Please start a yarp name server first";
        return 1;
    }

    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    auto remote = rf.check("remote", yarp::os::Value(DEFAULT_REMOTE_PORT));

    yarp::os::Property options {
        {"device", yarp::os::Value("speechSynthesizer_nwc_yarp")},
        {"local", yarp::os::Value("/speechSynthesizerClientExample/client")},
        {"remote", remote}
    };

    yarp::dev::PolyDriver device(options);

    if (!device.isValid())
    {
        yError() << "Failed to open device with options:" << options.toString();
        return 1;
    }

    yarp::dev::ISpeechSynthesizer * synthesizer = nullptr;

    if (!device.view(synthesizer))
    {
        yError() << "Failed to view ISpeechSynthesizer interface";
        return 1;
    }

    if (std::string language; !synthesizer->getLanguage(language))
    {
        yError() << "Failed to get current language";
        return 1;
    }
    else
    {
        yInfo() << "Current language:" << language;
    }

    if (std::string voice; !synthesizer->getVoice(voice))
    {
        yError() << "Failed to get current voice";
        return 1;
    }
    else
    {
        yInfo() << "Current voice:" << voice;
    }

    if (double speed; !synthesizer->getSpeed(speed))
    {
        yError() << "Failed to get current speed";
        return 1;
    }
    else
    {
        yInfo() << "Current speed:" << speed;
    }

    if (double pitch; !synthesizer->getPitch(pitch))
    {
        yError() << "Failed to get current pitch";
        // return 1; // not implemented by PiperSynthesizer
    }
    else
    {
        yInfo() << "Current pitch:" << pitch;
    }

    if (!synthesizer->setLanguage("english"))
    {
        yError() << "Failed to set language to English";
        return 1;
    }

    yInfo() << "Language set to English";

    if (!synthesizer->setSpeed(1.0))
    {
        yError() << "Failed to set speed to 1.0";
        return 1;
    }

    yInfo() << "Speed set to 1.0";

    yarp::sig::Sound sound;
    std::string text = "Hello, world! This is a test of the speech synthesizer.";

    if (!synthesizer->synthesize(text, sound))
    {
        yError() << "Failed to synthesize speech";
        return 1;
    }

    yarp::os::SystemClock::delaySystem(4.0); // Wait for synthesis to complete

    if (!synthesizer->setSpeed(0.5))
    {
        yError() << "Failed to set speed to 0.5";
        return 1;
    }

    yInfo() << "Speed set to 0.5";

    if (!synthesizer->synthesize(text, sound))
    {
        yError() << "Failed to synthesize speech after setting speed to 0.5";
        return 1;
    }

    yarp::os::SystemClock::delaySystem(6.0); // Wait for synthesis to complete

    if (!synthesizer->setSpeed(2.0))
    {
        yError() << "Failed to set speed to 2.0";
        return 1;
    }

    yInfo() << "Speed set to 2.0";

    if (!synthesizer->synthesize(text, sound))
    {
        yError() << "Failed to synthesize speech after setting speed to 2.0";
        return 1;
    }

    yarp::os::SystemClock::delaySystem(2.0); // Wait for synthesis to complete

    if (!synthesizer->setLanguage("spanish"))
    {
        yError() << "Failed to set language to Spanish";
        return 1;
    }

    yInfo() << "Language set to Spanish";

    if (!synthesizer->synthesize(text, sound))
    {
        yError() << "Failed to synthesize speech in Spanish";
        return 1;
    }

    yarp::os::SystemClock::delaySystem(4.0); // Wait for synthesis to complete

    if (!synthesizer->setVoice("lessac"))
    {
        yError() << "Failed to set voice to lessac";
        return 1;
    }

    yInfo() << "Voice set to lessac";

    if (!synthesizer->synthesize("Oh hello there!", sound))
    {
        yError() << "Failed to synthesize speech with lessac voice";
        return 1;
    }

    yarp::os::SystemClock::delaySystem(2.0); // Wait for synthesis to complete

    return 0;
}
