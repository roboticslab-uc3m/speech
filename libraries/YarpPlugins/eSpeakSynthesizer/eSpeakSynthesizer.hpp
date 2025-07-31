// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __ESPEAK_SYNTHESIZER_HPP__
#define __ESPEAK_SYNTHESIZER_HPP__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ISpeechSynthesizer.h>

#include "eSpeakSynthesizer_ParamsParser.h"

/**
 * @ingroup YarpPlugins
 * @defgroup eSpeakSynthesizer
 *
 * @brief Contains eSpeakSynthesizer.
 */

/**
 * @ingroup eSpeakSynthesizer
 * @brief The eSpeakSynthesizer class implements ISpeechSynthesizer.
 */
class eSpeakSynthesizer : public yarp::dev::DeviceDriver,
                          public yarp::dev::ISpeechSynthesizer,
                          public eSpeakSynthesizer_ParamsParser
{
public:
    // -- ISpeechSynthesizer declarations. Implementation in ISpeechSynthesizerImpl.cpp --
    yarp::dev::ReturnValue setLanguage(const std::string & language = "auto") override;
    yarp::dev::ReturnValue getLanguage(std::string & language) override;
    yarp::dev::ReturnValue setVoice(const std::string & voice_name = "auto") override;
    yarp::dev::ReturnValue getVoice(std::string & voice_name) override;
    yarp::dev::ReturnValue setSpeed(double speed = 0) override;
    yarp::dev::ReturnValue getSpeed(double & speed) override;
    yarp::dev::ReturnValue setPitch(double pitch) override;
    yarp::dev::ReturnValue getPitch(double & pitch) override;
    yarp::dev::ReturnValue synthesize(const std::string & text, yarp::sig::Sound & sound) override;

    // -------- DeviceDriver declarations. Implementation in DeviceDriverImpl.cpp --------
    bool open(yarp::os::Searchable & config) override;
    bool close() override;

private:
    bool isVoiceSet {false};
};

#endif // __ESPEAK_SYNTHESIZER_HPP__
