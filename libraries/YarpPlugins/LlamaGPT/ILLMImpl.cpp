// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "LlamaGPT.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

// ------------------- ILLM Related ------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::setPrompt(const std::string & prompt)
#else
bool LlamaGPT::setPrompt(const std::string & prompt)
#endif
{
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_method_not_implemented;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::readPrompt(std::string & oPrompt)
#else
bool LlamaGPT::readPrompt(std::string & oPrompt)
#endif
{
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_method_not_implemented;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::ask(const std::string & question, yarp::dev::LLM_Message & answer)
#else
bool LlamaGPT::ask(const std::string & question, yarp::dev::LLM_Message & answer)
#endif
{
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_method_not_implemented;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::getConversation(std::vector<yarp::dev::LLM_Message> & conversation)
#else
bool LlamaGPT::getConversation(std::vector<yarp::dev::LLM_Message> & conversation)
#endif
{
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_method_not_implemented;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::deleteConversation()
#else
bool LlamaGPT::deleteConversation()
#endif
{
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_method_not_implemented;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
yarp::dev::ReturnValue LlamaGPT::refreshConversation()
#else
bool LlamaGPT::refreshConversation()
#endif
{
#if YARP_VERSION_COMPARE(>=, 3, 12, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_method_not_implemented;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------
