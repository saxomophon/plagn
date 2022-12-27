/**
 *-------------------------------------------------------------------------------------------------
 * @file MqttClient.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the MqttClient class
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

#ifndef MQTTCLIENT_HPP
#define MQTTCLIENT_HPP

// own includes
#include "MqttInterface.hpp"
#include "TransportLayer.hpp"

class MqttClient : public MqttInterface
{
public:
    MqttClient(const Plag & parent, const std::string & brokerIP, unsigned int brokerPort,
                 const std::string & clientId, uint8_t defaultQoS, const std::string & userName,
                 const std::string & userPass, unsigned int keepAliveInterval, bool cleanSessions,
                 const std::string & willTopic, const std::string willMessage,
                 const std::vector<std::pair<std::string, uint8_t>> & defaultSubscriptions,
                 uint8_t version = 4);

    virtual void init();
    virtual void poll();
    virtual void transmitDatagram(std::shared_ptr<DatagramMqtt> datagram);

    virtual void connect();
    virtual bool isConnected();
    virtual void disconnect();

protected:
    virtual std::string createConnectMessage() = 0;

    // transmitter (from client to broker)
    virtual void transmitPingReq();

    // helper methods
    virtual void resendOldData();

protected:
    std::string m_clientId;             //!< this' id at the MQTT broker
    uint8_t m_qualityOfService;         //!< default quality of service flag for MQTT
    std::string m_userName;             //!< the client username provided to the Broker
    std::string m_userPass;             //!< password associated with m_userName
    unsigned int m_keepAliveInterval;   //!< keepaliveinterval in ms
    bool m_cleanSessions;               //!< whether or not to always start clean sessions
    std::string m_willTopic;            //!< topic, the will message will be published under (testament topic)
    std::string m_willMessage;          //!< message to be broadcast as will (testament)
    std::vector<std::pair<std::string, uint8_t>> m_defaultSubscriptions;    //!< subscriptions to subscribe to by default
    // working members
    const uint8_t m_protocolVersion;    //!< version of the protocol this client implemented
    bool m_brokerConnected;             //!< connection state of client to MQTT broker
    std::string m_transportBuffer;      //!< current buffer from TransportLayer
    std::unique_ptr<TransportLayer> m_transportLayer;   //!< connection interface
};

#endif /*MQTTCLIENT_HPP*/