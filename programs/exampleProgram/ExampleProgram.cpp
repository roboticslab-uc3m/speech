// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "ExampleProgram.hpp"

namespace teo
{

/************************************************************************/

bool ExampleProgram::configure(yarp::os::ResourceFinder &rf) {

    printf("--------------------------------------------------------------\n");
    if (rf.check("help")) {
        printf("ExampleProgram options:\n");
        printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        //printf("\t--file (default: \"%s\")\n",fileName.c_str());
    }

    printf("--------------------------------------------------------------\n");

    return true;
}

/************************************************************************/
double ExampleProgram::getPeriod() {
    return 2.0;  // Fixed, in seconds, the slow thread that calls updateModule below
}

/************************************************************************/
bool ExampleProgram::updateModule() {
    //printf("StateMachine in state [%d]. ExampleProgram alive...\n", stateMachine.getMachineState());
    return true;
}

/************************************************************************/

bool ExampleProgram::interruptModule() {
    return true;
}

/************************************************************************/

}  // namespace teo

