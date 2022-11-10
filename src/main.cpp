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
    shared_ptr<Plag> testPlag(new PlagUdp());

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
