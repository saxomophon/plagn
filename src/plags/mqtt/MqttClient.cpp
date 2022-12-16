/**
 * -------------------------------------------------------------------------------------------------
 * @file MqttClient.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements the MqttClient class
 * @version 0.1
 * @date 2022-12-07
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

// own includes
#include "TcpClient.hpp"
#include "TlsClient.hpp"

// self include
#include "MqttClient.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Mqtt Client:: Mqtt Client object assigns member values
 * 
 * @param parent reference to parent Plag
 * @param brokerIP ip of the broker as a string
 * @param brokerPort port of the broker to use
 * @param certFile path to certificate file for ssl connection
 * @param clientId this' id as presented to the broker
 * @param defaultQoS a default QoS level
 * @param userName username to supply to broker
 * @param userPass password to supply alongside username
 * @param keepAliveInterval keepalive interval in ms
 * @param cleanSessions flag, whether to start on a new session (true), or not (false)
 * @param willTopic topic of the testament
 * @param willMessage content of the testament
 * @param defaultSubscriptions default list of subsrciptions
 * @param version MQTT version of the client implementation
 */
MqttClient::MqttClient(const Plag & parent, const string & brokerIP, unsigned int brokerPort,
                       const string & certFile, const string & clientId, uint8_t defaultQoS,
                       const string & userName, const string & userPass,
                       unsigned int keepAliveInterval, bool cleanSessions,
                       const string & willTopic, const string willMessage,
                       const vector<std::pair<string, uint8_t>> & defaultSubscriptions,
                       uint8_t version):
    MqttInterface(parent, brokerIP, brokerPort),
    m_certFile(certFile),
    m_clientId(clientId),
    m_qualityOfService(defaultQoS),
    m_userName(userName),
    m_userPass(userPass),
    m_keepAliveInterval(keepAliveInterval),
    m_cleanSessions(cleanSessions),
    m_willTopic(willTopic),
    m_willMessage(willMessage),
    m_defaultSubscriptions(defaultSubscriptions),
    m_protocolVersion(version)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief MqttClient::init() configures the TransportLayer
 *
 */
void MqttClient::init() try
{
    if (startsWith('8', to_string(m_brokerPort)) || m_certFile != "")
    {
        m_transportLayer.reset(new TlsClient(std::chrono::milliseconds(1000),
                                             m_parent, m_brokerIP, m_brokerPort, m_certFile));
    }
    else
    {
        m_transportLayer.reset(new TcpClient(std::chrono::milliseconds(1000),
                                             m_parent, m_brokerIP, m_brokerPort));
    }
    connect();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClient::init()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief MqttClient::poll() checks the transport layer for data and resends old data
 *
 */
void MqttClient::poll() try
{
    resendOldData();
    if (m_transportLayer->getAvailableBytesCount() > 0)
    {
        m_transportBuffer += m_transportLayer->receiveBytes();
    }
    if (m_transportBuffer.size() > 0) parseIncomingBuffer(m_transportBuffer);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClient::init()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief transmits the Datagram to the broker
 * 
 * @param datagram a Datagram from parent
 */
void MqttClient::transmitDatagram(shared_ptr<DatagramMqtt> datagram) try
{
    string action = datagram->getAction();
    if (action == "publish")
    {
        uint8_t flags = (datagram->getQoS() & 0x03) << 1;
        flags |= datagram->getRetainFlag();
        transmitPublish(datagram->getTopic(), datagram->getContent(), flags);
    }
    else if (action == "subscribe")
    {
        transmitSubscribe(datagram->getTopic(), datagram->getQoS());
    }
    else if (action == "unsubscribe")
    {
        transmitUnsubscribe(datagram->getTopic());
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClient::transmitDatagram()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief generalized connect sequence
 * 
 */
void MqttClient::connect() try
{
    m_transportLayer->connect(std::chrono::milliseconds(2500));
    if (!m_transportLayer->isConnected()) throw std::runtime_error("Could not connect as TCP client!");

    m_transportLayer->transmit(createConnectMessage());
    m_lastTimeOfSent = std::chrono::steady_clock::now();
    // wait for CONNACK to happen
    if (!m_transportLayer->isConnected()) throw std::runtime_error("Could not connect as TCP client!");
    string connAckMsg = "";
    connAckMsg = m_transportLayer->receiveBytes(4);
    if (connAckMsg.at(0) == static_cast<char>(CONNACK << 4))
    {
        uint8_t offset;
        unsigned int neededLength = readMqttVarInt(connAckMsg, offset, 1);
        if (neededLength > 2) connAckMsg += m_transportLayer->receiveBytes(neededLength - 2);
        parseConnAck(connAckMsg);
        if (!m_brokerConnected)
        {
            m_transportLayer->disconnect();
            cout << "Diconnected because of CONNACK failure!" << endl;
        }
        else
        {
            m_lastTimeReceived = std::chrono::steady_clock::now();
            for (const std::pair<string, uint8_t> & subscription : m_defaultSubscriptions)
            {
                transmitSubscribe(subscription.first, subscription.second);
            }
        }
    }
    else
    {
        cout << "Error in CONNACK message: " << getBinStringAsAsciiHex(connAckMsg) << endl;
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClient::connect()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief current connection state
 * 
 * @return true, if this and TransportLayer are conneceted
 * @return false, else
 */
bool MqttClient::isConnected() try
{
    if (m_lastTimeReceived < m_lastTimeOfSent
        && m_lastTimeOfSent - m_lastTimeReceived > std::chrono::seconds(2 * m_keepAliveInterval))
    {
        cout << "Disconnect because of timeout" << endl;
        disconnect();
    }
    bool isConnected = m_transportLayer->isConnected() && m_brokerConnected;
    if (isConnected
        && std::chrono::steady_clock::now() - m_lastTimeOfSent > std::chrono::seconds(m_keepAliveInterval))
    {
        transmitPingReq();
    }
    return isConnected;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClient::isConnected()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief generalized disconnect sequence
 * 
 */
void MqttClient::disconnect() try
{
    if (m_transportLayer->isConnected()) this->transmitDisconnect();
    m_brokerConnected = false;
    m_transportLayer->disconnect();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClient::disconnect()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a PINGREQ (ping request) telegram
 * @details It is the responsibility of the client to regularly send data. This is done by the
 * PINGREQ. Afterwards the broker will reply with a PINGREP (ping reply) to indicate, that it is
 * still available.
 */
void MqttClient::transmitPingReq() try
{
    string data = "";

    prepareFixedHeader(PINGREQ, 0, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClient::transmitPingReq()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief convenvience function to resend packets in the loop, where the time demands it
 * @details some packets (those with QoS > 0) require acknowledgements and should be resent, if
 * that acknowledgement fails to come. Therefore we'll hold that packet with current time information
 * in a map, tied to its identifier, for quick access. This function checks the map's items' time
 * points, whether a resend is in order. If so, it does resend it and updates the time point.
 * @sa MqttInterface::addNonAcknowledgedData()
 * @sa MqttInterface::hasMessages()
 */
void MqttClient::resendOldData() try
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    // keyValPair is a pair of types uint16_t and a pair of string and time_point
    for (auto & keyValPair : m_nonAckedData)
    {
        if (now > keyValPair.second.second + std::chrono::milliseconds(m_keepAliveInterval))
        {
            m_transportLayer->transmit(keyValPair.second.first);
            keyValPair.second.second = std::chrono::steady_clock::now();
        }
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClient::resendOldData()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}