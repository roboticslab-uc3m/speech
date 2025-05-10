// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "LlamaGPT.hpp"

#include <cctype> // std::isspace

#include <algorithm> // std::find_if

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

namespace
{
    // https://stackoverflow.com/a/217605

    // trim from start (in place)
    inline void ltrim(std::string & s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
        {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    inline void rtrim(std::string & s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
        {
            return !std::isspace(ch);
        }).base(), s.end());
    }
}

// ------------------- ILLM Related ------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::setPrompt(const std::string & prompt)
#else
bool LlamaGPT::setPrompt(const std::string & prompt)
#endif
{
    if (!m_prompt.empty())
    {
        yCError(LLAMA) << "Prompt already set";
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    auto temp = prompt;
    ltrim(temp);
    rtrim(temp);

    if (temp.empty())
    {
        yCWarning(LLAMA) << "Requested prompt is empty";
    }

    yCInfo(LLAMA) << "Setting prompt:" << temp;
    m_prompt = temp;
    conversation.push_back(yarp::dev::LLM_Message("system", m_prompt, {}, {}));

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::readPrompt(std::string & oPrompt)
#else
bool LlamaGPT::readPrompt(std::string & oPrompt)
#endif
{
    oPrompt = m_prompt;
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::ask(const std::string & question, yarp::dev::LLM_Message & answer)
#else
bool LlamaGPT::ask(const std::string & question, yarp::dev::LLM_Message & answer)
#endif
{
    yCInfo(LLAMA) << "Asking:" << question;
    conversation.push_back(yarp::dev::LLM_Message("user", question, {}, {}));

    auto prompt = m_prompt;

    if (!prompt.empty())
    {
        prompt += " ";
    }

    prompt += question;

    const llama_vocab * vocab = llama_model_get_vocab(model);

    // tokenize the prompt

    // find the number of tokens in the prompt
    const int n_prompt = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), nullptr, 0, true, true);

    // allocate space for the tokens and tokenize the prompt
    std::vector<llama_token> prompt_tokens(n_prompt);

    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), true, true) < 0)
    {
        yCError(LLAMA) << "Failed to tokenize the prompt";
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    // initialize the context

    llama_context_params ctx_params = llama_context_default_params();
    // context size
    ctx_params.n_ctx = n_prompt + m_tokens - 1;
    // maximum number of tokens that can be processed in a single call to llama_decode
    ctx_params.n_batch = n_prompt;
    // disable performance counters
    ctx_params.no_perf = true;

    llama_context * ctx = llama_init_from_model(model, ctx_params);

    if (!ctx)
    {
        yCError(LLAMA) << "Failed to create context";
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    // initialize the sampler

    auto sparams = llama_sampler_chain_default_params();
    sparams.no_perf = true;

    llama_sampler * smpl = llama_sampler_chain_init(sparams);

    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());

    // prepare a batch for the prompt
    llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());

    llama_token new_token_id;
    std::string out;

    for (int n_pos = 0; n_pos + batch.n_tokens < n_prompt + m_tokens; )
    {
        // evaluate the current batch with the transformer model
        if (llama_decode(ctx, batch))
        {
            yCError(LLAMA) << "Failed to call llama_decode()";
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
            return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
            return false;
#endif
        }

        n_pos += batch.n_tokens;

        // sample the next token
        {
            new_token_id = llama_sampler_sample(smpl, ctx, -1);

            // is it an end of generation?
            if (llama_vocab_is_eog(vocab, new_token_id))
            {
                break;
            }

            char buf[128];
            int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);

            if (n < 0)
            {
                yCError(LLAMA) << "Failed to convert token to piece";
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
                return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
                return false;
#endif
            }

            std::string s(buf, n);
            out += s;

            // prepare the next batch with the sampled token
            batch = llama_batch_get_one(&new_token_id, 1);
        }
    }

    llama_sampler_free(smpl);
    llama_free(ctx);

    yCDebug(LLAMA) << "Generated:" << out;
    auto index = out.find("</think>");

    if (index != std::string::npos)
    {
        out = out.substr(index + 8);
        ltrim(out);
        rtrim(out);
    }

    answer = {"assistant", out, {}, {}};
    conversation.push_back(answer);

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::getConversation(std::vector<yarp::dev::LLM_Message> & conversation)
#else
bool LlamaGPT::getConversation(std::vector<yarp::dev::LLM_Message> & conversation)
#endif
{
    conversation = this->conversation;
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::deleteConversation()
#else
bool LlamaGPT::deleteConversation()
#endif
{
    yCInfo(LLAMA) << "Deleting conversation and prompt";
    conversation.clear();
    m_prompt.clear();
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::refreshConversation()
#else
bool LlamaGPT::refreshConversation()
#endif
{
    yCInfo(LLAMA) << "Deleting conversation while keeping the prompt";
    conversation.clear();
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------
