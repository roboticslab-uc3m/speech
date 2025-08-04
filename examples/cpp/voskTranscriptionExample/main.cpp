// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup speech-examples-cpp
 * @defgroup voskTranscriptionExample voskTranscriptionExample
 */

#include <iostream>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/TypedReaderCallback.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ISpeechTranscription.h>

constexpr auto DEFAULT_MODEL = "vosk-model-small-es-0.42";
constexpr auto DEFAULT_PORT_NAME = "/voskTranscriptionExample/sound:i";

class SoundCallback : public yarp::os::TypedReaderCallback<yarp::sig::Sound>
{
public:
    SoundCallback(yarp::dev::ISpeechTranscription * transcription)
        : transcription(transcription)
    {}

private:
    yarp::dev::ISpeechTranscription * transcription {nullptr};
};

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

    yarp::os::Property options {
        {"device", yarp::os::Value("VoskTranscription")},
        {"model", rf.check("model", yarp::os::Value(DEFAULT_MODEL))},
        {"useGPU", rf.check("useGPU", yarp::os::Value(false))},
    };

    yarp::dev::PolyDriver device(options);

    if (!device.isValid())
    {
        yError() << "Failed to open device with options:" << options.toString();
        return 1;
    }

    yarp::dev::ISpeechTranscription * transcription = nullptr;

    if (!device.view(transcription))
    {
        yError() << "Failed to view ISpeechTranscription interface";
        return 1;
    }

    if (std::string language; !transcription->getLanguage(language))
    {
        yError() << "Failed to get current language";
        return 1;
    }
    else
    {
        yInfo() << "Current language:" << language;
    }

    SoundCallback callback(transcription);
    yarp::os::BufferedPort<yarp::sig::Sound> soundPort;
    soundPort.useCallback(callback);

    if (!soundPort.open(rf.check("port", yarp::os::Value(DEFAULT_PORT_NAME)).asString()))
    {
        yError() << "Failed to open sound port";
        return 1;
    }

    do
    {
        std::cout << "Press ENTER to quit" << std::endl;
    }
    while (std::cin.get() != '\n');

    soundPort.interrupt();
    soundPort.close();

    return 0;
}
