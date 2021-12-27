// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __ESPEAK_HPP__
#define __ESPEAK_HPP__

#include "TextToSpeechIDL.h"

namespace roboticslab
{

/**
 * @ingroup speech-libraries
 * @defgroup Espeak
 * @brief Contains roboticslab::Espeak.
 */

/**
 * @ingroup Espeak
 * @brief Implements TextToSpeechIDL.
 */
class Espeak : public TextToSpeechIDL
{
public:

    Espeak();

    ~Espeak() override;

    bool setLanguage(const std::string & language) override;

    return_string_list getSupportedLangs() override;

    bool say(const std::string & text) override;

    bool setSpeed(int speed) override;

    bool setPitch(int pitch) override;

    return_int_param getSpeed() override;

    return_int_param getPitch() override;

    bool play() override;

    bool stop() override;

    bool pause() override;

    bool checkSayDone() override;

};

} // namespace roboticslab

#endif // __ESPEAK_HPP__
