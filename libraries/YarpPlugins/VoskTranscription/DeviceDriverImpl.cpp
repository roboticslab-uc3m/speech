// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "VoskTranscription.hpp"

#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>

#include "LogComponent.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool VoskTranscription::open(yarp::os::Searchable & config)
{
    if (!parseParams(config))
    {
        yCError(VOSK) << "Failed to parse parameters";
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool VoskTranscription::close()
{
    return true;
}

// -----------------------------------------------------------------------------
