/**
 *-------------------------------------------------------------------------------------------------
 * @file main.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @brief Core of Plag'n for orchestration
 * @version 0.1
 * @date 2022-05-20
 *
 * @copyright Copyright (c) 2022, licensed unter LGPL v2.1
 *
 * Targets of chosen license for:
 *      Users    : Please be so kind as to indicate your usage of this library by linking to the project
 *                 page, currently being: https://github.com/saxomophon/plagn
 *      Devs     : Your improvements to the code, should be available publicly under the same license.
 *                 That way, anyone will benefit from it.
 *      Corporate: Even you are either a User or a Developer. No charge will apply, no guarantee or
 *                 warranty will be given.
 *
 */

// std includes
#include <iostream>
#include <memory>

// own includes
#include "PlagUdp.hpp"

using namespace std;

// Global
bool gotTerminationRequest = false; //!< representing a termination request from outside (e.g. ctrl c)

void receiveTermination(int /*unused*/)
{
    std::cout << "Received termination!" << endl;
    gotTerminationRequest = true;
}

int main()
{
    cout << "Hello World!" << endl;

    // phase of construction an file reads
    shared_ptr<Plag> testPlag(new PlagUdp("testPlag", 1));

    // init phase

    bool atLeastOneThreadRunning = true;

    testPlag->init();

    // event Handling:
#ifdef _WIN32
    typedef void (*cancelEventHandlePointer)(int);
    cancelEventHandlePointer sigterm = signal(SIGTERM, receiveTermination);
    cancelEventHandlePointer sigint = signal(SIGINT, receiveTermination);
#else
    struct sigaction externalCancelEvent;
    memset(&externalCancelEvent, 0, sizeof(externalCancelEvent));
    externalCancelEvent.sa_handler = receiveTermination; // register setter function for own cancel evaluation term
    sigemptyset(&externalCancelEvent.sa_mask);
    externalCancelEvent.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &externalCancelEvent, nullptr); // register signal for termination types bash kill and killall
    sigaction(SIGINT, &externalCancelEvent, nullptr);  // register signal for interruption bash ctrl+c
#endif

    // running phase

    testPlag->startWorker();

    while (atLeastOneThreadRunning && !gotTerminationRequest)
    {
        atLeastOneThreadRunning = false;
        // test for all threads
        if (true /*insert test on Plag if still working*/) atLeastOneThreadRunning = true;
    }

    // stopping phase
    testPlag->stopWork();

    cout << "Done. See you World!" << endl;

    return 0;
}
