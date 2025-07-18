// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __VOSK_TRANSCRIPTION_HPP__
#define __VOSK_TRANSCRIPTION_HPP__

#include <string>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ISpeechTranscription.h>

#include <vosk_api.h>

#include "VoskTranscription_ParamsParser.h"

/**
 * @ingroup YarpPlugins
 * @defgroup VoskTranscription
 *
 * @brief Contains VoskTranscription.
 */

/**
 * @ingroup VoskTranscription
 * @brief The VoskTranscription class implements ISpeechTranscription.
 */
class VoskTranscription : public yarp::dev::DeviceDriver,
                          public yarp::dev::ISpeechTranscription,
                          public VoskTranscription_ParamsParser
{
public:
    // -- ISpeechTranscription declarations. Implementation in ISpeechTranscriptionImpl.cpp --
    yarp::dev::ReturnValue setLanguage(const std::string & language = "auto") override;
    yarp::dev::ReturnValue getLanguage(std::string & language) override;
    yarp::dev::ReturnValue transcribe(const yarp::sig::Sound & sound, std::string & transcription, double & score) override;

    // -------- DeviceDriver declarations. Implementation in DeviceDriverImpl.cpp --------
    bool open(yarp::os::Searchable & config) override;
    bool close() override;

private:
    std::string modelName;

    VoskModel * model {nullptr};
    VoskRecognizer * recognizer {nullptr};

    int sampleRate {0};
};

#endif // __VOSK_TRANSCRIPTION_HPP__
