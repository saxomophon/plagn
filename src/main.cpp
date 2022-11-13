/**
 *-------------------------------------------------------------------------------------------------
 * @file main.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Core of Plag'n for orchestration
 * @version 0.1
 * @date 2022-05-20
 *
 * @copyright LGPL v2.1
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
#include <string>

// boost incldues
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

// own includes
#include "PlagUdp.hpp"
#include "Utilities.hpp"

using namespace std;

// Global
bool gotTerminationRequest = false; //!< representing a termination request from outside (e.g. ctrl c)

void receiveTermination(int /*unused*/)
{
    cout << "Received termination!" << endl;
    gotTerminationRequest = true;
}

void printVersionToCommandLine()
{
    // string topBottomLine = "-------------------------------";
    // string program = APP_PRODUCT"Plag'n";
    // program = string("|") + string((topBottomLine.size() - program.size() - 2) / 2, " ")
    //           + program + string((topBottomLine.size() - program.size() - 2) / 2, " ")
    //           + string((topBottomLine.size() - program.size() - 2) % 2 == 0 ? "|" : " |");
    // string version = APP_VERSION;
    // version = string("|") + string((topBottomLine.size() - version.size() - 2) / 2, " ")
    //           + version + string((topBottomLine.size() - version.size() - 2) / 2, " ")
    //           + string((topBottomLine.size() - version.size() - 2) % 2 == 0 ? "|" : " |");
    // string website = "github.com/saxomophon/plagn/";
    // website = string("|") + string((topBottomLine.size() - website.size() - 2) / 2, " ")
    //           + website + string((topBottomLine.size() - website.size() - 2) / 2, " ")
    //           + string((topBottomLine.size() - website.size() - 2) % 2 == 0 ? "|" : " |");
    // string copyright = APP_COPYRIGHT;
    // copyright = string("|") + string((topBottomLine.size() - copyright.size() - 2) / 2, " ")
    //             + copyright + string((topBottomLine.size() - copyright.size() - 2) / 2, " ")
    //             + string((topBottomLine.size() - copyright.size() - 2) % 2 == 0 ? "|" : " |");
    
    // cout << topBottomLine << endl;
    // cout << "|   This program is called:   |" << endl;
    // cout << program << endl;
    // cout << "|" << string(topBottomLine.size(), " ") << "|" << endl;
    // cout << "|       Its version is        |" << endl;
    // cout << version << endl;
    // cout << "|" << string(topBottomLine.size(), " ") << "|" << endl;
    // cout << "| It's provided under license |" << endl;
    // cout << copyright << endl;
    // cout << "|" << string(topBottomLine.size(), " ") << "|" << endl;
    // cout << "|     Find out more under     |" << endl;
    // cout << topBottomLine << endl;
    cout << "-------------------------------" << endl;
    cout << "|   This program is called:   |" << endl;
    cout << "|          Plag'n             |" << endl;
    cout << "|                             |" << endl;
    cout << "|       Its version is        |" << endl;
    cout << "|            0.0.1            |" << endl;
    cout << "|                             |" << endl;
    cout << "| It's provided under license |" << endl;
    cout << "|          LGPL v2.1          |" << endl;
    cout << "|                             |" << endl;
    cout << "|     Find out more under     |" << endl;
    cout << "| github.com/saxomophon/plagn/|" << endl;
    cout << "-------------------------------" << endl;
}

int main(int argc, char * argv[])
{
    cout << "Hello World!" << endl;

    printVersionToCommandLine();

    string configFilePath = "";
    cout << "options:" << endl;
    for (int i = 0; i < argc; i++)
    {
        cout << argv[i] << endl;
    }
    if (argc == 2)
    {
        configFilePath = string(argv[1]);
    }
    if (configFilePath == "") configFilePath = plagn::DEFAULT_CONFIG_PATH;
    cout << "Using config file: \"" << configFilePath << "\"" << endl;

    // phase of construction an file reads

    // reading the file
    
    boost::property_tree::ptree propertyTree;
    boost::property_tree::ini_parser::read_ini(configFilePath, propertyTree);

    // constructing the Plags
    
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
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    // stopping phase
    testPlag->stopWork();

    cout << "Done. See you World!" << endl;

    return 0;
}
