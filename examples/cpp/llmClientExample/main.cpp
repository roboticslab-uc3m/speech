// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup speech-examples-cpp
 * @defgroup llmClientExample llmClientExample
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ILLM.h>

constexpr auto DEFAULT_REMOTE_PORT = "/llm_nws/rpc:i";

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
        {"device", yarp::os::Value("LLM_nwc_yarp")},
        {"local", yarp::os::Value("/llmClientExample/client")},
        {"remote", remote}
    };

    yarp::dev::PolyDriver device(options);

    if (!device.isValid())
    {
        yError() << "Failed to open device with options:" << options.toString();
        return 1;
    }

    yarp::dev::ILLM * llm = nullptr;

    if (!device.view(llm))
    {
        yError() << "Failed to view ILLM interface";
        return 1;
    }

    if (!llm->setPrompt("You are a helpful mathematician."))
    {
        yError() << "Failed to set prompt";
        return 1;
    }

    if (std::string prompt; !llm->readPrompt(prompt))
    {
        yError() << "Failed to read prompt";
        return 1;
    }
    else
    {
        yInfo() << "Prompt is:" << prompt;
    }

    yarp::dev::LLM_Message answer;

    std::string question = "What is 2 + 2?";

    yInfo() << "Asking:" << question;

    if (!llm->ask(question, answer))
    {
        yError() << "Failed to ask question";
        return 1;
    }

    yInfo() << "Answer:";
    yInfo() << "* type:" << answer.type;
    yInfo() << "* content:" << answer.content;
    yInfo() << "* parameters:" << answer.parameters;
    yInfo() << "* arguments:" << answer.arguments;

    yInfo() << "Deleting conversation";

    if (!llm->refreshConversation())
    {
        yError() << "Failed to refresh conversation";
        return 1;
    }

    question = "What is 3 + 3?";

    yInfo() << "Asking:" << question;

    if (!llm->ask(question, answer))
    {
        yError() << "Failed to ask question";
        return 1;
    }

    yInfo() << "Answer:";
    yInfo() << "* type:" << answer.type;
    yInfo() << "* content:" << answer.content;
    yInfo() << "* parameters:" << answer.parameters;
    yInfo() << "* arguments:" << answer.arguments;

    if (std::vector<yarp::dev::LLM_Message> conversation; !llm->getConversation(conversation))
    {
        yError() << "Failed to get conversation";
        return 1;
    }
    else
    {
        yInfo() << "Conversation:";

        for (const auto & msg : conversation)
        {
            yInfo() << msg.toString();
        }
    }

    yInfo() << "Deleting conversation and prompt";

    if (!llm->deleteConversation())
    {
        yError() << "Failed to delete conversation";
        return 1;
    }

    if (!llm->setPrompt("You are the worst mathematician ever, you always provide wrong answers."))
    {
        yError() << "Failed to set prompt";
        return 1;
    }

    if (std::string prompt; !llm->readPrompt(prompt))
    {
        yError() << "Failed to read prompt";
        return 1;
    }
    else
    {
        yInfo() << "Prompt is:" << prompt;
    }

    question = "What is 2 + 2?";

    yInfo() << "Asking:" << question;

    if (!llm->ask(question, answer))
    {
        yError() << "Failed to ask question";
        return 1;
    }

    yInfo() << "Answer:";
    yInfo() << "* type:" << answer.type;
    yInfo() << "* content:" << answer.content;
    yInfo() << "* parameters:" << answer.parameters;
    yInfo() << "* arguments:" << answer.arguments;

    return 0;
}
