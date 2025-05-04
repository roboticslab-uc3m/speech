// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "LlamaGPT.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool LlamaGPT::open(yarp::os::Searchable & config)
{
    if (!parseParams(config))
    {
        yCError(LLAMA) << "Failed to parse parameters";
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool LlamaGPT::close()
{
    return true;
}

// -----------------------------------------------------------------------------
