/**
 *-------------------------------------------------------------------------------------------------
 * @file MqttClientV4.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the MqttClientV4 class
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

#ifndef MQTTCLIENTV4_HPP
#define MQTTCLIENTV4_HPP

// own includes
#include "MqttClient.hpp"

class MqttClientV4 : public MqttClient
{
public:
    MqttClientV4(const Plag & parent, const std::string & brokerIP, unsigned int brokerPort,
                 const std::string & certFile, const std::string & clientId, uint8_t defaultQoS,
                 const std::string & userName, const std::string & userPass,
                 unsigned int keepAliveInterval, bool cleanSessions,
                 const std::string & willTopic, const std::string willMessage,
                 const std::vector<std::pair<std::string, uint8_t>> & defaultSubscriptions);

protected:
    virtual std::string createConnectMessage();

    virtual void parseConnect(const std::string & content);
    virtual void parseConnAck(const std::string & content);
    virtual void parseDisconnect(const std::string & content);
    virtual void parsePublish(uint8_t firstByte, std::string & content);
    virtual void parsePubAck(const std::string & content);
    virtual void parsePubRec(const std::string & content);
    virtual void parsePubRel(const std::string & content);
    virtual void parsePubComp(const std::string & content);
    virtual void parseSubAck(const std::string & content);
    virtual void parseUnsubAck(const std::string & content);
    virtual void parseAuth(const std::string & content);

    // transmitter (from client to broker)
    virtual void transmitDisconnect();
    virtual void transmitAuth(bool reauthenticate);
    virtual void transmitPublish(const std::string & topic, const std::string & content, uint8_t flags);
    virtual void transmitPubAck(const std::string & identifier, char reasonCode = '\x00');
    virtual void transmitPubRec(const std::string & identifier, char reasonCode = '\x00');
    virtual void tranmitPubRel(const std::string & identifier, char reasonCode = '\x00');
    virtual void transmitPubComp(const std::string & identifier, char reasonCode = '\x00');
    virtual void transmitSubscribe(const std::string & topic, uint8_t qos);
    virtual void transmitUnsubscribe(const std::string & topic);

protected:
};

#endif /*MQTTCLIENTV4_HPP*/