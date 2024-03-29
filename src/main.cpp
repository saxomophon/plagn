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
#include "PlagMqtt.hpp"
#include "PlagUdp.hpp"
#include "PlagHttpServer.hpp"
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
    cout << "|           Plag'n            |" << endl;
    cout << "|                             |" << endl;
    cout << "|       Its version is        |" << endl;
    cout << "|            0.1.0            |" << endl;
    cout << "|                             |" << endl;
    cout << "| It's provided under license |" << endl;
    cout << "|          LGPL v2.1          |" << endl;
    cout << "|                             |" << endl;
    cout << "|     Find out more under     |" << endl;
    cout << "| github.com/saxomophon/plagn |" << endl;
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
    map<string, shared_ptr<Plag>> allPlags;
    bool stillHasPlags = true;
    string plagKey;
    size_t index = 1;
    do
    {
        plagKey = string("plag") + to_string(index);
        stillHasPlags = (propertyTree.find(plagKey) != propertyTree.not_found());
        if (stillHasPlags)
        {
            string type = PropertyTreeReader::getParameter<string>(propertyTree, plagKey, "type");
            string name = PropertyTreeReader::getParameter<string>(propertyTree, plagKey, "name");
            cout << "Found Plag at index " << index << " with type " << type << " and name " << name << endl;
            if (type == "mqtt")
            {
                shared_ptr<Plag> sharedPlag(new PlagMqtt(propertyTree, name, index));
                allPlags.insert_or_assign(name, sharedPlag);
            }
            else if (type == "udp")
            {
                shared_ptr<Plag> sharedPlag(new PlagUdp(propertyTree, name, index));
                allPlags.insert_or_assign(name, sharedPlag);
            }
            else if (type == "httpserver")
            {
                shared_ptr<Plag> sharedPlag(new PlagHttpServer(propertyTree, name, index));
                allPlags.insert_or_assign(name, sharedPlag);
            }
            index++;
        }
    } while (stillHasPlags);


    // constructing the Plags
    map<string, shared_ptr<Kable>> allKables;
    bool stillHasKables = true;
    string kableKey;
    index = 1;
    do
    {
        kableKey = string("kable") + to_string(index);
        stillHasKables = (propertyTree.find(kableKey) != propertyTree.not_found());
        if (stillHasKables)
        {
            string source = PropertyTreeReader::getParameter<string>(propertyTree, kableKey,
                                                                     "sourcePlag");
            string target = PropertyTreeReader::getParameter<string>(propertyTree, kableKey,
                                                                     "targetPlag");
            cout << "Found Kable at index " << index << " to go from " << source << " to " << target << endl;

            if (allPlags.count(source) == 0 || allPlags.count(target) == 0)
            {
                cerr << "Your Kable kable" << index << " has non-existent source(\"" << source
                     << "\") or target(\"" << target << "\"). -> Exiting!" << endl;
                exit(EXIT_FAILURE);
            }

            allKables.insert_or_assign(source, shared_ptr<Kable>(new Kable(propertyTree, kableKey,
                                                                           allPlags.at(source),
                                                                           allPlags.at(target))));
            allPlags.at(source)->attachKable(allKables.at(source));

            index++;
        }
    } while (stillHasKables);
    

    // init phase

    bool atLeastOneThreadRunning = true;

    cout << "Initializing Plags..." << endl;
    for (const pair<string, shared_ptr<Plag>> & plagEntry : allPlags)
    {
        plagEntry.second->init();
    }
    cout << "Initialized all Plags." << endl;

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

    // running phass
    cout << "Starting Plags..." << endl;
    for (const pair<string, shared_ptr<Plag>> & plagEntry : allPlags)
    {
        plagEntry.second->startWorker();
    }
    cout << "Plags running." << endl;

    while (atLeastOneThreadRunning && !gotTerminationRequest)
    {
        atLeastOneThreadRunning = false;
        // test for all threads
        if (true /*insert test on Plag if still working*/) atLeastOneThreadRunning = true;
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    cout << "Stopping Plags..." << endl;
    // stopping phase
    for (const pair<string, shared_ptr<Plag>> & plagEntry : allPlags)
    {
        plagEntry.second->stopWork();
    }
    cout << "Plags stoped." << endl;

    cout << "Done. See you World!" << endl;

    return 0;
}
