/**
 *-------------------------------------------------------------------------------------------------
 * @file MqttInterface.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the MqttInterface abstract class
 * @version 0.1
 * @date 2022-12-06
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

#ifndef MQTTINTERFACE_HPP
#define MQTTINTERFACE_HPP

// std includes
#include <list>

// own includes
#include "DatagramMqtt.hpp"
#include "Plag.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief base of every MQTT handler (be it client or broker, version 4 or 5)
 * 
 */
class MqttInterface
{
public:
    MqttInterface(const Plag & parent, const std::string & brokerIP,
                  unsigned int brokerPort);

    virtual void init() = 0;
    virtual void poll() = 0;

    virtual void transmitDatagram(const std::shared_ptr<DatagramMqtt> datagram) = 0;
    virtual bool hasMessages();
    virtual std::shared_ptr<DatagramMqtt> getMessage();

protected:
    // mqtt string helper
    virtual std::string makeMqttString(const std::string & text) const;
    virtual std::string extractMqttString(std::string & text, size_t startPos = 0) const;
    // datagram generation convenience stuff
    virtual void prepareFixedHeader(MqttMessageType type, uint8_t flags,
                                    std::string & content) const;

    // parser (from broker to client (this))
    virtual std::size_t parseIncomingBuffer(std::string & inBuffer);

    // general helper
    uint16_t generateIdentifier();
    void addNonAcknowledgedData(uint16_t identifier, const std::string & data);
    void removeNonAcknowledgedData(uint16_t identifier);

    // parser (all abstract)
    virtual void parseConnect(const std::string & content) = 0;
    virtual void parseConnAck(const std::string & content) = 0;
    virtual void parsePublish(uint8_t firstByte, std::string & content) = 0;
    virtual void parsePubAck(const std::string & content) = 0;
    virtual void parsePubRec(const std::string & content) = 0;
    virtual void parsePubRel(const std::string & content) = 0;
    virtual void parsePubComp(const std::string & content) = 0;
    virtual void parseSubAck(const std::string & content) = 0;
    virtual void parseUnsubAck(const std::string & content) = 0;
    virtual void parseAuth(const std::string & content) = 0;

    // transmitter (all abstract)
    virtual void transmitAuth(bool reauthenticate) = 0;
    virtual void transmitPublish(const std::string & topic, const std::string & content, uint8_t flags) = 0;
    virtual void transmitPubAck(const std::string & identifier, char reasonCode = '\x00') = 0;
    virtual void transmitPubRec(const std::string & identifier, char reasonCode = '\x00') = 0;
    virtual void tranmitPubRel(const std::string & identifier, char reasonCode = '\x00') = 0;
    virtual void transmitPubComp(const std::string & identifier, char reasonCode = '\x00') = 0;
    virtual void transmitSubscribe(const std::string & topic, uint8_t qos) = 0;
    virtual void transmitUnsubscribe(const std::string & topic) = 0;
    virtual void transmitPingReq() = 0;

protected:
    const Plag & m_parent;                                          //!< reference to parent plag
    std::string m_brokerIP;                                         //!< ip for the broker service
    unsigned int m_brokerPort;                                      //!< port to use with the broker service
    uint16_t m_currentIdentifier;                                   //!< packet identifier
    std::list<std::shared_ptr<DatagramMqtt>> m_incomingBuffer;      //!< buffer of parsed but not yet forwarded telegrams
    std::chrono::steady_clock::time_point m_lastTimeOfSent;         //!< indicator of last time this sent stuff to broker
    std::chrono::steady_clock::time_point m_lastTimeReceived;       //!< indicator of last time this received from broker
    std::map<uint16_t, std::pair<std::string, std::chrono::steady_clock::time_point>> m_nonAckedData; //!< data transmitted but not yet acknowledged by broker
};

#endif /*MQTTINTERFACE_HPP*/