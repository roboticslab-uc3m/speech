#include "gtest/gtest.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ISpeechSynthesizer.h>

namespace roboticslab::test
{

/**
 * @ingroup speech-tests
 * @brief Tests eSpeakSynthesizer.
 */
class eSpeakSynthesizerTest : public testing::Test
{
public:
    void SetUp() override
    {
        options = {
            {"device", yarp::os::Value("eSpeakSynthesizer")}
        };
    }

    void TearDown() override
    {
        ASSERT_TRUE(synthesizerDevice.close());
    }

protected:
    yarp::os::Property options;
    yarp::dev::PolyDriver synthesizerDevice;
    yarp::dev::ISpeechSynthesizer * synthesizer;

    std::string language, voice;
    double speed, pitch;
    yarp::sig::Sound sound;

    static constexpr yarp::dev::ReturnValue::return_code ok {yarp::dev::ReturnValue::return_code::return_value_ok};
    static constexpr yarp::dev::ReturnValue::return_code not_ready {yarp::dev::ReturnValue::return_code::return_value_error_not_ready};
};

TEST_F(eSpeakSynthesizerTest, eSpeakSynthesizerVoiceNotSet)
{
    ASSERT_TRUE(synthesizerDevice.open(options));
    ASSERT_TRUE(synthesizerDevice.view(synthesizer));

    ASSERT_EQ(synthesizer->getLanguage(language), not_ready);
    ASSERT_EQ(synthesizer->getVoice(voice), not_ready);
    ASSERT_EQ(synthesizer->getSpeed(speed), not_ready);
    ASSERT_EQ(synthesizer->getPitch(pitch), not_ready);
    ASSERT_EQ(synthesizer->synthesize("Hello, world!", sound), not_ready);
}

TEST_F(eSpeakSynthesizerTest, eSpeakSynthesizerGetters)
{
    options.put("voice", yarp::os::Value("mb-en1"));

    ASSERT_TRUE(synthesizerDevice.open(options));
    ASSERT_TRUE(synthesizerDevice.view(synthesizer));

    ASSERT_EQ(synthesizer->getLanguage(language), ok);
    ASSERT_EQ(language, "en-uk");

    ASSERT_EQ(synthesizer->getVoice(voice), ok);
    ASSERT_EQ(voice, "english-mb-en1");

    ASSERT_EQ(synthesizer->getSpeed(speed), ok);
    ASSERT_GT(speed, 0.0);

    ASSERT_EQ(synthesizer->getPitch(pitch), ok);
    ASSERT_GE(pitch, 0.0);
    ASSERT_LE(pitch, 1.0);

    ASSERT_EQ(synthesizer->synthesize("Hello, world!", sound), ok);
    ASSERT_GT(sound.getSamples(), 0);
}

TEST_F(eSpeakSynthesizerTest, eSpeakSynthesizerSetters)
{
    options.put("voice", yarp::os::Value("mb-en1"));

    ASSERT_TRUE(synthesizerDevice.open(options));
    ASSERT_TRUE(synthesizerDevice.view(synthesizer));

    ASSERT_EQ(synthesizer->setLanguage("es"), ok);
    ASSERT_EQ(synthesizer->getLanguage(language), ok);
    ASSERT_EQ(language, "es");

    ASSERT_EQ(synthesizer->setVoice("mb-es1"), ok);
    ASSERT_EQ(synthesizer->getVoice(voice), ok);
    ASSERT_EQ(voice, "spanish-mbrola-1");

    ASSERT_EQ(synthesizer->setSpeed(0.5), ok);
    ASSERT_EQ(synthesizer->getSpeed(speed), ok);
    ASSERT_NEAR(speed, 0.5, 0.01);

    ASSERT_EQ(synthesizer->setPitch(0.75), ok);
    ASSERT_EQ(synthesizer->getPitch(pitch), ok);
    ASSERT_NEAR(pitch, 0.75, 0.01);

    ASSERT_EQ(synthesizer->synthesize("¡Hola, mundo!", sound), ok);
    ASSERT_GT(sound.getSamples(), 0);
}

} // namespace roboticslab::test
