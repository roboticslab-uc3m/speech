// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __PIPER_SYNTHESIZER_HPP__
#define __PIPER_SYNTHESIZER_HPP__

#include <filesystem>
#include <string>
#include <unordered_map>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ISpeechSynthesizer.h>

#include <piper/piper.h>

#include "PiperSynthesizer_ParamsParser.h"

/**
 * @ingroup YarpPlugins
 * @defgroup PiperSynthesizer
 *
 * @brief Contains PiperSynthesizer.
 */

/**
 * @ingroup PiperSynthesizer
 * @brief The PiperSynthesizer class implements ISpeechSynthesizer.
 */
class PiperSynthesizer : public yarp::dev::DeviceDriver,
                         public yarp::dev::ISpeechSynthesizer,
                         public PiperSynthesizer_ParamsParser
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
    struct model_entry
    {
        std::string path;
        std::string code;
        std::string language;
        std::string dataset;
    };

    bool inspectModels(const std::filesystem::path & base);
    void loadCurrentModel();

    std::unordered_map<std::string, model_entry> storage;
    model_entry * current_model {nullptr};

    piper_synthesizer * synth {nullptr};
    piper_synthesize_options options;

    std::string eSpeakDataFullPath;
};

#endif // __PIPER_SYNTHESIZER_HPP__
