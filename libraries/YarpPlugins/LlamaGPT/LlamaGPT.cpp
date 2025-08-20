// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "LlamaGPT.hpp"

#include <cstdlib> // std::free
#include <cstring> // std::strcmp

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

// -----------------------------------------------------------------------------

bool LlamaGPT::clear(bool preservePrompt, bool recreateContext)
{
    if (!messages.empty())
    {
        if (preservePrompt)
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
        else
        {
            for (auto & msg : messages)
            {
                std::free(const_cast<char *>(msg.content));
            }

            messages.clear();
        }
    }

    prev_len = 0;

    if (recreateContext && ctx)
    {
        llama_free(ctx);

        // initialize the context
        llama_context_params ctx_params = llama_context_default_params();
        ctx_params.n_ctx = m_tokens;
        ctx_params.n_batch = m_tokens;
        ctx_params.no_perf = false;

        ctx = llama_init_from_model(model, ctx_params);

        if (!ctx)
        {
            yCError(LLAMA) << "Failed to recreate context";
            return 1;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------
