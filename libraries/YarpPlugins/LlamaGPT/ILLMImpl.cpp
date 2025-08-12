// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "LlamaGPT.hpp"

#include <cctype> // std::isspace
#include <cstdlib> // std::free
#include <cstring> // std::strcmp, ::strdup (POSIX standard, but not C standard)

#include <algorithm> // std::find_if, std::transform
#include <iterator> // std::back_inserter

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
    if (!conversation.empty())
    {
        yCError(LLAMA) << "Conversation has started or the prompt was already set, you must delete the conversation first";
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    auto temp = prompt;
    ltrim(temp);
    rtrim(temp);

    if (!temp.empty())
    {
        yCInfo(LLAMA) << "Setting prompt:" << temp;
        conversation.push_back({"system", ::strdup(temp.c_str())});
    }
    else
    {
        yCWarning(LLAMA) << "Requested prompt is empty, not setting it";
    }

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
    if (!conversation.empty() && conversation.front().role == "system")
    {
        oPrompt = conversation.front().content;
    }
    else
    {
        yCWarning(LLAMA) << "No prompt set, returning empty string";
        oPrompt.clear();
    }

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
    conversation.push_back({"user", ::strdup(question.c_str())});

    std::string prompt;

    if (!conversation.empty() && conversation.front().role == "system")
    {
        prompt = conversation.front().content;
    }

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
    conversation.push_back({"assistant", ::strdup(out.c_str())});

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
    conversation.clear();
    conversation.reserve(this->conversation.size());

    std::transform(this->conversation.cbegin(), this->conversation.cend(), std::back_inserter(conversation),
        [](const llama_chat_message & msg)
        {
            return yarp::dev::LLM_Message(msg.role, msg.content, {}, {});
        });

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

    for (auto & msg : conversation)
    {
        std::free(const_cast<char *>(msg.content));
    }

    conversation.clear();
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
    yCInfo(LLAMA) << "Deleting conversation while keeping the prompt (if any)";

    if (!conversation.empty())
    {
        auto first = conversation.end();

        for (auto it = conversation.begin(); it != conversation.end(); ++it)
        {
            if (std::strcmp(it->role, "system") != 0)
            {
                std::free(const_cast<char *>(it->content));
            }
            else if (first == conversation.end())
            {
                first = it;
            }
        }

        conversation.erase(first, conversation.end());
    }
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------
