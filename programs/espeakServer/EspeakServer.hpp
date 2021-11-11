// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __ESPEAK_SERVER_HPP__
#define __ESPEAK_SERVER_HPP__

#include <yarp/os/RFModule.h>
#include <yarp/os/RpcServer.h>

#include "Espeak.hpp"

namespace roboticslab
{

/**
 * @ingroup espeakServer
 * @brief An RPC service wrapper around roboticslab::Espeak.
 */
class EspeakServer : public yarp::os::RFModule
{
public:
    bool configure(yarp::os::ResourceFinder & rf) override;

    bool updateModule() override
    { return true; }

    bool interruptModule() override
    { port.interrupt(); return true; }

private:
    yarp::os::RpcServer port;
    Espeak espeak;
};

} // namespace roboticslab

#endif // __ESPEAK_SERVER_HPP__
