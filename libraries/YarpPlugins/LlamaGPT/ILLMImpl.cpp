// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "LlamaGPT.hpp"

#include <cctype> // std::isspace
#include <cstring> // ::strdup (POSIX standard, but not C standard)

#include <algorithm> // std::find_if, std::transform
#include <iostream>
#include <iterator> // std::back_inserter
#include <regex>

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

    std::string trim(const std::string & s)
    {
        std::string out = s;
        ltrim(out);
        rtrim(out);
        return out;
    }

    std::regex re("\\\\boxed\\{([^}]+)\\}");
}

// ------------------- ILLM Related ------------------------------------

yarp::dev::ReturnValue LlamaGPT::setPrompt(const std::string & prompt)
{
    if (!messages.empty())
    {
        yCError(LLAMA) << "Conversation has started or the prompt was already set, you must delete the conversation first";
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    const auto trimmed = trim(prompt);

    if (!trimmed.empty())
    {
        yCInfo(LLAMA) << "Setting prompt:" << trimmed;
        messages.push_back({"system", ::strdup(trimmed.c_str())});
    }
    else
    {
        yCWarning(LLAMA) << "Requested prompt is empty, not setting it";
    }

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue LlamaGPT::readPrompt(std::string & oPrompt)
{
    if (!messages.empty() && messages.front().role == std::string("system"))
    {
        oPrompt = messages.front().content;
    }
    else
    {
        yCWarning(LLAMA) << "No prompt set, returning empty string";
        oPrompt.clear();
    }

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue LlamaGPT::ask(const std::string & question, yarp::dev::LLM_Message & answer)
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
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
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
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
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
            return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
        }

        // evaluate the current batch with the transformer model
        if (llama_decode(ctx, batch))
        {
            yCError(LLAMA) << "Failed to call llama_decode()";
            return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
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
            return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
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

    yCDebug(LLAMA, "Decoded %d tokens in %.2f s, speed: %.2f t/s",
            n_decode, (t_main_end - t_main_start) / 1000000.0f, n_decode / ((t_main_end - t_main_start) / 1000000.0f));

    llama_perf_sampler_print(smpl);
    llama_perf_context_print(ctx);

    if (auto index = out.find("</think>"); index != std::string::npos)
    {
        out = trim(out.substr(index + 8));
    }

    if (auto index = out.find("<think>"); index != std::string::npos)
    {
        out = trim(out.substr(index + 7));
    }

    if (std::smatch sm; std::regex_search(out, sm, re) && !sm.empty())
    {
        out = sm[1].str();
    }

    yCInfo(LLAMA) << "Response:" << out;

    answer = {"assistant", out, {}, {}};
    messages.push_back({"assistant", ::strdup(out.c_str())});

    prev_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), false, nullptr, 0);

    if (prev_len < 0)
    {
        yCError(LLAMA) << "Failed to apply the chat template (post)";
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
    }

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue LlamaGPT::getConversation(std::vector<yarp::dev::LLM_Message> & conversation)
{
    conversation.clear();
    conversation.reserve(messages.size());

    std::transform(messages.cbegin(), messages.cend(), std::back_inserter(conversation),
        [](const llama_chat_message & msg)
        {
            return yarp::dev::LLM_Message(msg.role, msg.content, {}, {});
        });

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue LlamaGPT::deleteConversation()
{
    yCInfo(LLAMA) << "Deleting conversation and prompt";

    return clear(false)
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
}

// -----------------------------------------------------------------------------

yarp::dev::ReturnValue LlamaGPT::refreshConversation()
{
    yCInfo(LLAMA) << "Deleting conversation while keeping the prompt (if any)";

    return clear(true)
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
}

// -----------------------------------------------------------------------------
