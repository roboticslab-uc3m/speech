// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __LLAMA_GPT_HPP__
#define __LLAMA_GPT_HPP__

#include <vector>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ILLM.h>

#include <llama.h>

#include "LlamaGPT_ParamsParser.h"

/**
 * @ingroup YarpPlugins
 * @defgroup LlamaGPT
 *
 * @brief Contains LlamaGPT.
 */

/**
 * @ingroup LlamaGPT
 * @brief The LlamaGPT class implements ILLM.
 */
class LlamaGPT : public yarp::dev::DeviceDriver,
                 public yarp::dev::ILLM,
                 public LlamaGPT_ParamsParser
{
public:
    // -- ILLM declarations. Implementation in ILLMImpl.cpp --
    yarp::dev::ReturnValue setPrompt(const std::string & prompt) override;
    yarp::dev::ReturnValue readPrompt(std::string & oPrompt) override;
    yarp::dev::ReturnValue ask(const std::string & question, yarp::dev::LLM_Message & answer) override;
    yarp::dev::ReturnValue getConversation(std::vector<yarp::dev::LLM_Message> & conversation) override;
    yarp::dev::ReturnValue deleteConversation() override;
    yarp::dev::ReturnValue refreshConversation() override;

    // -------- DeviceDriver declarations. Implementation in DeviceDriverImpl.cpp --------
    bool open(yarp::os::Searchable & config) override;
    bool close() override;

private:
    bool clear(bool preservePrompt, bool recreateContext = true);

    llama_model * model {nullptr};
    llama_context * ctx {nullptr};
    llama_sampler * smpl {nullptr};
    const char * tmpl {nullptr};

    std::vector<llama_chat_message> messages;
    int prev_len {0}; // length of the previous message in the conversation
};

#endif // __LLAMA_GPT_HPP__
