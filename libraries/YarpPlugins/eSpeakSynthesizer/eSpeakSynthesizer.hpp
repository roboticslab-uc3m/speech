// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __ESPEAK_SYNTHESIZER_HPP__
#define __ESPEAK_SYNTHESIZER_HPP__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ISpeechSynthesizer.h>

namespace roboticslab
{

/**
 * @ingroup YarpPlugins
 * @defgroup eSpeakSynthesizer
 *
 * @brief Contains roboticslab::eSpeakSynthesizer.
 */

/**
 * @ingroup eSpeakSynthesizer
 * @brief The eSpeakSynthesizer class implements ISpeechSynthesizer.
 */
class eSpeakSynthesizer : public yarp::dev::DeviceDriver,
                          public yarp::dev::ISpeechSynthesizer
{
public:
    // -- ISpeechSynthesizer declarations. Implementation in ISpeechSynthesizerImpl.cpp --
    bool setLanguage(const std::string & language = "auto") override;
    bool getLanguage(std::string & language) override;
    bool setVoice(const std::string & voice_name = "auto") override;
    bool getVoice(std::string & voice_name) override;
    bool setSpeed(const double speed = 0) override;
    bool getSpeed(double & speed) override;
    bool setPitch(const double pitch) override;
    bool getPitch(double & pitch) override;
    bool synthesize(const std::string & text, yarp::sig::Sound & sound) override;

    // -------- DeviceDriver declarations. Implementation in DeviceDriverImpl.cpp --------
    bool open(yarp::os::Searchable & config) override;
    bool close() override;

private:
};

} // namespace roboticslab

#endif // __ESPEAK_SYNTHESIZER_HPP__
