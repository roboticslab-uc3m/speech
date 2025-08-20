// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "LlamaGPT.hpp"

#include <cctype> // std::isspace
#include <cstdlib> // std::free
#include <cstring> // std::strcmp, ::strdup (POSIX standard, but not C standard)

#include <algorithm> // std::find_if, std::transform
#include <iostream>
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
    if (!messages.empty())
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
        messages.push_back({"system", ::strdup(temp.c_str())});
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
    if (!messages.empty() && messages.front().role == "system")
    {
        oPrompt = messages.front().content;
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

    std::vector<char> formatted(llama_n_ctx(ctx));

    // add the user input to the message list and format it
    messages.push_back({"user", ::strdup(question.c_str())});

    int new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, formatted.data(), formatted.size());

    if (new_len > (int)formatted.size())
    {
        formatted.resize(new_len);
        new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, formatted.data(), formatted.size());
    }

    if (new_len < 0)
    {
        yCError(LLAMA) << "Failed to apply the chat template (pre)";
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    // remove previous messages to obtain the prompt to generate the response
    std::string prompt(formatted.begin() + prev_len, formatted.begin() + new_len);

    const bool is_first = llama_memory_seq_pos_max(llama_get_memory(ctx), 0) == -1;

    const llama_vocab * vocab = llama_model_get_vocab(model);

    // find the number of tokens in the prompt
    const int n_prompt = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), nullptr, 0, is_first, true);

    // allocate space for the tokens and tokenize the prompt
    std::vector<llama_token> prompt_tokens(n_prompt);

    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), is_first, true) < 0)
    {
        yCError(LLAMA) << "Failed to tokenize the prompt";
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

    const auto t_main_start = ggml_time_us();
    int n_decode = 0;

    while (true)
    {
        // check if we have enough space in the context to evaluate this batch
        int n_ctx = llama_n_ctx(ctx);
        int n_ctx_used = llama_memory_seq_pos_max(llama_get_memory(ctx), 0) + 1;

        if (n_ctx_used + batch.n_tokens > n_ctx)
        {
            yCError(LLAMA) << "Context size exceeded: " << n_ctx_used + batch.n_tokens << " > " << n_ctx;
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
            return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
            return false;
#endif
        }

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

        // sample the next token
        new_token_id = llama_sampler_sample(smpl, ctx, -1);

        // is it an end of generation?
        if (llama_vocab_is_eog(vocab, new_token_id))
        {
            break;
        }

        char buf[256];
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

        if (m_print)
        {
            std::cout << s << std::flush;
        }

        // prepare the next batch with the sampled token
        batch = llama_batch_get_one(&new_token_id, 1);

        n_decode++;
    }

    if (m_print)
    {
        std::cout << std::endl;
    }

    const auto t_main_end = ggml_time_us();

    yCDebug(LLAMA) << "Generated:" << out;

    yCDebug(LLAMA, "Decoded %d tokens in %.2f s, speed: %.2f t/s",
            n_decode, (t_main_end - t_main_start) / 1000000.0f, n_decode / ((t_main_end - t_main_start) / 1000000.0f));

    llama_perf_sampler_print(smpl);
    llama_perf_context_print(ctx);

    if (auto index = out.find("</think>"); index != std::string::npos)
    {
        out = out.substr(index + 8);
        ltrim(out);
        rtrim(out);
    }

    answer = {"assistant", out, {}, {}};
    messages.push_back({"assistant", ::strdup(out.c_str())});

    prev_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), false, nullptr, 0);

    if (prev_len < 0)
    {
        yCError(LLAMA) << "Failed to apply the chat template (post)";
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

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
    conversation.reserve(messages.size());

    std::transform(messages.cbegin(), messages.cend(), std::back_inserter(conversation),
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

    for (auto & msg : messages)
    {
        std::free(const_cast<char *>(msg.content));
    }

    messages.clear();
    prev_len = 0;
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

    if (!messages.empty())
    {
        auto first = messages.end();

        for (auto it = messages.begin(); it != messages.end(); ++it)
        {
            if (std::strcmp(it->role, "system") != 0)
            {
                std::free(const_cast<char *>(it->content));
            }
            else if (first == messages.end())
            {
                first = it;
            }
        }

        messages.erase(first, messages.end());
    }

    prev_len = 0;
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------
