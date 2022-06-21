#include "gtest/gtest.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/SystemClock.h>

#include "Espeak.hpp"

namespace roboticslab::test
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
    ASSERT_EQ(espeak.getSpeed(), speed);

    auto pitch = 69;
    ASSERT_TRUE(espeak.setPitch(pitch));
    ASSERT_EQ(espeak.getPitch(), pitch);

    auto languages = espeak.getSupportedLangs();
    ASSERT_NE(languages.size(), 0);

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

} // namespace roboticslab::test
