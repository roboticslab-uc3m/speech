#include "gtest/gtest.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/SystemClock.h>

#include "Espeak.hpp"

namespace roboticslab
{

/**
 * @ingroup speech-tests
 * @brief Tests roboticslab::Espeak.
 */
class EspeakTest : public testing::Test
{
protected:
    Espeak espeak;
};

TEST_F(EspeakTest, EspeakBasicConfig)
{
    auto speed = 100;
    ASSERT_TRUE(espeak.setSpeed(speed));
    ASSERT_TRUE(espeak.getSpeed().ret);
    ASSERT_EQ(espeak.getSpeed().param, speed);

    auto pitch = 69;
    ASSERT_TRUE(espeak.setPitch(pitch));
    ASSERT_TRUE(espeak.getPitch().ret);
    ASSERT_EQ(espeak.getPitch().param, pitch);

    auto languages = espeak.getSupportedLangs();
    ASSERT_TRUE(languages.ret);
    ASSERT_NE(languages.string_list.size(), 0);

    ASSERT_TRUE(espeak.checkSayDone());
}

TEST_F(EspeakTest, EspeakSayShort)
{
    ASSERT_TRUE(espeak.setLanguage("mb-en1"));

    auto text = "Hello, world!";
    ASSERT_TRUE(espeak.say(text));

    do
    {
        yarp::os::SystemClock::delaySystem(0.1);
    }
    while (!espeak.checkSayDone());
}

TEST_F(EspeakTest, EspeakSayLong)
{
    ASSERT_TRUE(espeak.setLanguage("mb-en1"));

    auto text = "Hello, world! This is a very long sentence. It is supposed to be long enough to be read by the espeak engine.";
    ASSERT_TRUE(espeak.say(text));
    ASSERT_FALSE(espeak.checkSayDone());

    yarp::os::SystemClock::delaySystem(2.0);

    ASSERT_FALSE(espeak.checkSayDone());
    ASSERT_TRUE(espeak.stop());

    yarp::os::SystemClock::delaySystem(1.0); // this is necessary for CI

    ASSERT_TRUE(espeak.checkSayDone());
}

}// namespace roboticslab
