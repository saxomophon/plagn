/**
 *-------------------------------------------------------------------------------------------------
 * @file MqttClientV5.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the MqttClientV5 class
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

#ifndef MQTTCLIENTV5_HPP
#define MQTTCLIENTV5_HPP

// own includes
#include "MqttClient.hpp"

enum MqttReasonCode: uint8_t
{
    SUCCESS = 0x00,
    VERSION_REFUSED = 0x01,
    ID_REJECTED = 0x02,
    SERVICE_UNAVAILABLE = 0x03,
    BAD_USER_CRED = 0x04,
    CLIENT_PROHIBITED = 0x05,
    NO_SUBS = 0x10,
    NO_SUBS_EXISTED = 0x11,
    UNSPECIFIED_ERROR = 0x80,
    MALFORMED_PACKET = 0x81,
    PROTOCOL_ERROR = 0x82,
    BROKER_ERROR = 0x83,
    UNSUPPORTED_PROTOCOL = 0x84,
    INVALID_ID = 0x85,
    BAD_USER_CRED_V5 = 0x86,
    NOT_AUTHORIZED = 0x87,
    SERVER_UNAVAILABLE_V5 = 0x88,
    SERVER_BUSY = 0x89,
    BANNED = 0x8A,
    SERVER_SHUTS_DOWN = 0x8B,
    BAD_AUTH_METHOD = 0x8C,
    KEEP_ALIVE_TIMEOUT = 0x8D,
    SESSION_TAKEOVER = 0x8E,
    INVALID_TOPIC_FILTER = 0x8F,
    INVALID_TOPIC_NAME = 0x90,
    PACKET_ID_IN_USE = 0x91,
    PACKET_ID_NOT_FOUND = 0x92,
    RECEIVE_MAX_EXCEEDED = 0x93,
    INVALID_TOPIC_ALIAS = 0x94,
    PACKET_TOO_LARGE = 0x95,
    MESSAGE_RATE_TOO_HIGH = 0x96,
    QUOTA_EXCEEDED = 0x97,
    ADMIN_ACTION = 0x98,
    PAYLOAD_FORMAT_INVALID = 0x99,
    NO_RETAIN = 0x9A,
    NO_QOS = 0x9B,
    USE_ANOTHER = 0x9C,
    SERVER_MOVED = 0x9D,
    SHARED_SUB_NOT_SUPPORTED = 0x9E,
    CONNECTION_RATE_EXCEEDED = 0x9F,
    MAX_CONNECT_TIME = 0xA0,
    SUB_ID_NOT_SUPPORTED = 0xA1,
    SUB_WILDCARD_NOT_SUPPORTED = 0xA2
};

class MqttClientV5 : public MqttClient
{
public:
    MqttClientV5(const Plag & parent, const std::string & brokerIP, unsigned int brokerPort,
                 const std::string & certFile, const std::string & clientId, uint8_t defaultQoS,
                 const std::string & userName, const std::string & userPass,
                 unsigned int keepAliveInterval, bool cleanSessions,
                 const std::string & willTopic, const std::string willMessage,
                 const std::vector<std::pair<std::string, uint8_t>> & defaultSubscriptions);

protected:
    // mqtt string helper
    std::map<MqttPropertyType, std::string> readProperties(const std::string & content) const;
    std::map<std::string,
             std::string> convertUserPropertiesToMap(const std::map<MqttPropertyType,
                                                                    std::string> & properties) const;
   // datagram generation convenience stuff
    std::string makeProperty(MqttPropertyType type, const DataType & data) const;

    virtual std::string createConnectMessage();

    // parser (from broker to client (this))
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
    uint16_t m_sessionId;                   //!< id to session
    std::string m_authenticationType;   //!< method to use for authentication
    std::string m_authenticationData;   //!< data to use with authentication
    unsigned int m_sessionExpire;       //!< when a session should expire (0 = on disconnect, UINT_MAX = never)
    bool m_willIsText;                      //!< whether will is text or number
    unsigned int m_willDelay;               //!< delay before the will message will be sent; default 0
};

#endif /*MQTTCLIENTV5_HPP*/