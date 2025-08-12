// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "LlamaGPT.hpp"

#include <fstream>
#include <sstream>

#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#include "LogComponent.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool LlamaGPT::open(yarp::os::Searchable & config)
{
    if (!parseParams(config))
    {
        yCError(LLAMA) << "Failed to parse parameters";
        return false;
    }

    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("LlamaGPT");

    auto modelFullPath = rf.findFileByName(m_model);

    if (modelFullPath.empty())
    {
        yCError(LLAMA) << "Model file not found:" << m_model;
        return false;
    }

    yCInfo(LLAMA) << "Loading model from:" << modelFullPath;

    if (m_ngl <= 0)
    {
        yCError(LLAMA) << "Invalid number of GPU layers" << m_ngl;
        return false;
    }

    if (m_tokens <= 0)
    {
        yCError(LLAMA) << "Invalid number of tokens to predict" << m_tokens;
        return false;
    }

    // load dynamic backends
    ggml_backend_load_all();

    // initialize the model
    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = m_ngl;

    model = llama_model_load_from_file(modelFullPath.c_str(), model_params);

    if (!model)
    {
        yCError(LLAMA) << "Failed to load model";
        return false;
    }

    const llama_vocab * vocab = llama_model_get_vocab(model);

    if (!m_prompt.empty())
    {
        if (!setPrompt(m_prompt))
        {
            return false;
        }
    }
    else if (!m_promptFile.empty())
    {
        yCInfo(LLAMA) << "Loading prompt from:" << m_promptFile;

        auto promptFullPath = rf.findFileByName(m_promptFile);

        if (promptFullPath.empty())
        {
            yCError(LLAMA) << "Prompt file not found";
            return false;
        }

        std::ifstream promptFile(promptFullPath);

        if (!promptFile.is_open())
        {
            yCError(LLAMA) << "Failed to open prompt file";
            return false;
        }

        std::stringstream buffer;
        buffer << promptFile.rdbuf();
        auto prompt = buffer.str();

        if (!setPrompt(prompt))
        {
            return false;
        }
    }

    // initialize the sampler
    smpl = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());

    return true;
}

// -----------------------------------------------------------------------------

bool LlamaGPT::close()
{
    bool ret = deleteConversation();

    if (smpl)
    {
        llama_sampler_free(smpl);
        smpl = nullptr;
    }

    if (model)
    {
        llama_model_free(model);
        model = nullptr;
    }

    return ret;
}

// -----------------------------------------------------------------------------
