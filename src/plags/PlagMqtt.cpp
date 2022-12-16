/**
 *-------------------------------------------------------------------------------------------------
 * @file PlagMqtt.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements the PlagMqtt class
 * @version 0.1
 * @date 2022-11-29
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

// std include
#include <iostream>

// boost includes
#include <boost/algorithm/string.hpp>

// own includes
#include "DatagramMqtt.hpp"
#include "MqttClientV4.hpp"
#include "MqttClientV5.hpp"

// self include
#include "PlagMqtt.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new PlagMqtt::PlagMqtt object assigns default values
 * 
 */
PlagMqtt::PlagMqtt(const boost::property_tree::ptree & propTree,
                   const std::string & name, const uint64_t & id) :
    Plag(propTree, name, id, PlagType::MQTT)
{
    readConfig();
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Destroy the PlagMqtt::PlagMqtt object closes socket, in addition to regular destruct
 * 
 */
PlagMqtt::~PlagMqtt()
{
    if (!m_stopToken) stopWork();
    try
    {
        if (m_mqttVersion < 5)
        {
            shared_ptr<MqttClientV4> client = dynamic_pointer_cast<MqttClientV4>(m_interface);
            client->disconnect();
        }
    }
    catch (exception & e)
    {
        cerr << "Could not close PlagMqtt, because of " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Could not close PlagMqtt, because for unknown reason!" << endl;
    }
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief reads many optional parameters from config and assigns them to member values
 * 
 */
void PlagMqtt::readConfig() try
{
    m_mqttVersion = getOptionalParameter<uint8_t>("version", 4);
    
    m_brokerIP = getParameter<string>("brokerIP");
    m_port = getOptionalParameter<uint16_t>("port", 1883);

    m_keepAliveInterval = getOptionalParameter<unsigned int>("keepAliveInterval", 300);
    m_certFile = getOptionalParameter<string>("certFile", string(""));

    m_userName = getOptionalParameter<string>("userName", string("plagn"));
    m_userPass = getOptionalParameter<string>("userPass", string("plagn"));

    m_cleanSessions = getOptionalParameter<bool>("cleanSessions", false);

    string subscriptionsList = getOptionalParameter<string>("subscriptions", "");
    vector<string> subscriptionsPairs;
    if (subscriptionsList.size() == 0) return;
    boost::split(subscriptionsPairs, subscriptionsList, boost::is_any_of(";"));
    for (const string & subPair : subscriptionsPairs)
    {
        vector<string> subscriptionsPair;
        boost::split(subscriptionsPair, subPair, boost::is_any_of(","));
        if (subscriptionsPair.size() != 2)
        {
            throw std::invalid_argument("Incomplete subscription in settings: \"" + subPair + "\"");
        }
        m_defaultSubscriptions.push_back(std::pair<string, uint8_t>(subscriptionsPair.at(0),
                                                                    stoul(subscriptionsPair.at(1),
                                                                          nullptr, 0)));
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagMqtt::readConfig()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagMqtt::init() configures the MQTT interface
 * 
 */
void PlagMqtt::init() try
{
    if (m_mqttVersion <= 4)
    {
        string emptyString = "";
        m_interface = shared_ptr<MqttClientV4>(new MqttClientV4(*this, m_brokerIP, m_port, m_certFile,
                                                                "plagn", 0, m_userName, m_userPass,
                                                                m_keepAliveInterval, m_cleanSessions,
                                                                emptyString, emptyString,
                                                                m_defaultSubscriptions));
    }
    else if (m_mqttVersion == 5)
    {

        string emptyString = "";
        m_interface = shared_ptr<MqttClientV5>(new MqttClientV5(*this, m_brokerIP, m_port, m_certFile,
                                                                "plagn", 0, m_userName, m_userPass,
                                                                m_keepAliveInterval, m_cleanSessions,
                                                                emptyString, emptyString,
                                                                m_defaultSubscriptions));
    }
    m_interface->init();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagMqtt::init()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagMqtt::loopWork regularly reads on the socket, if data popped in or sends data
 * 
 */
bool PlagMqtt::loopWork() try
{
    if (m_mqttVersion <= 5)
    {
        shared_ptr<MqttClient> client = dynamic_pointer_cast<MqttClient>(m_interface);
        if (!client->isConnected())
        {
            // try to connect, when disconnected
            client->connect();
            return true;
        }
        else
        {
            bool somethingDone = false;
            // send Datagrams
            if (client->hasMessages())
            {
                appendToDistribution(client->getMessage());
                somethingDone = true;
            }

            // send MQTT messages
            if (m_incommingDatagrams.begin() != m_incommingDatagrams.end())
            {
                shared_ptr<DatagramMqtt> castPtr;
                castPtr = dynamic_pointer_cast<DatagramMqtt>(m_incommingDatagrams.front());
                if (castPtr == nullptr)
                {
                    m_incommingDatagrams.pop_front();
                    return somethingDone;
                }
                client->transmitDatagram(castPtr);
                m_incommingDatagrams.pop_front();
                somethingDone = true;
            }
            return somethingDone;
        }
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened during PlagMqtt::loopWork()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief placeDatagram is a function to place a Datagram here. Hence this method decodes the
 * datagram
 *
 * @param datagram A Datagram containing data for this Plag to interprete
 */
void PlagMqtt::placeDatagram(const shared_ptr<Datagram> datagram) try
{
    const shared_ptr<DatagramMqtt> castPtr = dynamic_pointer_cast<DatagramMqtt>(datagram);
    if (castPtr != nullptr)
    {
        m_incommingDatagrams.push_back(datagram);
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagMqtt::placeDatagram()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}
