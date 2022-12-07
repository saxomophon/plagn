/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramMqtt.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the DatagramMqtt class
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

#ifndef DATAGRAMMQTT_HPP
#define DATAGRAMMQTT_HPP

// std includes

// own includes
#include "Datagram.hpp"


/**
 * -------------------------------------------------------------------------------------------------
 * @brief short hand as used by the MQTT protocol (see defintion)
 *
 */
enum MqttMessageType : uint8_t
{
    CONNECT = 1,   //!< Client request to connect to Server
    CONNACK = 2,   //!< Connect acknowledgment
    PUBLISH = 3,   //!< Publish message
    PUBACK = 4,   //!< Publish acknowledgment
    PUBREC = 5,   //!< Publish received (assured delivery part 1)
    PUBREL = 6,   //!< Publish release (assured delivery part 2)
    PUBCOMP = 7,   //!< Publish complete (assured delivery part 3)
    SUBSCRIBE = 8,   //!< Client subscribe request
    SUBACK = 9,   //!< Subscribe acknowledgment
    UNSUBSCRIBE = 10,   //!< Unsubscribe request
    UNSUBACK = 11,   //!< Unsubscribe acknowledgment
    PINGREQ = 12,   //!< PING request
    PINGREP = 13,   //!< PING response
    DISCONNECT = 14,   //!< Client is disconnecting
    AUTH = 15,   //!< Authentication exchange (since MQTT 5.0)
    FORBIDDEN = 0    //!< Reserved
};

/**
 *-------------------------------------------------------------------------------------------------
 * @brief short hand for property flags as used from MQTT v.5 onwards
 * 
 */
enum MqttPropertyType : uint8_t
{
    FORMAT = 0x01, //!< Followed by byte
    EXPIRES = 0x02, //!< Four Byte Int
    CONTENT_TYPE = 0x03, //!< MQTT string
    RESPONSE_TOPIC = 0x08, //!< MQTT String
    CORRELATION_DATA = 0x09, //!< Raw Data
    SUBSCRIPTION_ID = 0x0B, //!< Variable Byte Int
    SESSION_EXPIRE = 0x11, //!< Four Byte Int
    OWN_CLIENT_ID = 0x12, //!< MQTT String
    KEEP_ALIVE = 0x13, //!< Two Byte Int
    AUTH_METHOD = 0x15, //!< MQTT String
    AUTH_DATA = 0x16, //!< Raw data
    REQUEST_PROBLEM_INFO = 0x17, //!< Byte
    WILL_DELAY = 0x18, //!< Four Byte Int
    REQUEST_RESPONSE_INFO = 0x19, //!< Byte
    RESPONSE_INFO = 0x1A, //!< MQTT String
    SERVER_REF = 0x1C, //!< MQTT String
    REASON = 0x1F, //!< MQTT String
    RECEIVE_MAX = 0x21, //!< Two Byte Int
    TOPIC_ALIAS_MAX = 0x22, //!< Two Byte Int
    TOPIC_ALIAS = 0x23, //!< Two Byte Int
    QOS_MAX = 0x24, //!< Byte
    RETAIN_AVAILABLE = 0x25, //!< Byte
    USER_PROPERTY = 0x26, //!< MQTT String Pair
    PACKET_SIZE_MAX = 0x27, //!< Four Byte Int
    WILDCARD_SUB_AVAILABLE = 0x28, //!< Byte
    SUBSCRIPTION_ID_AVAILABLE = 0x29, //!< Byte
    SHARED_SUBSCRIPTION_AVAILABLE = 0x2A, //!< Byte
};

class DatagramMqtt : public Datagram
{
public:
    DatagramMqtt(const std::string & sourcePlag);
    DatagramMqtt(const std::string & sourcePlag, const std::string & action,
                 const std::string & topic, const std::string & content, uint8_t qos, bool retain);

    const std::string & getAction() const;
    const std::string & getTopic() const;
    const std::string & getContent() const;
    const std::string & getUserInfo() const;
    unsigned int getQoS() const;
    bool getRetainFlag() const;

    virtual DataType getData(const std::string & key) const;

    virtual void setData(const std::string & key, const DataType & value);

    virtual std::string toString() const;


private:
    std::string m_action;   //!< action of the module/protocol (e.g. SUBSCRIBE, PUBLISH, etc.)
    std::string m_topic;    //!< topic to subscribe to/to publish under
    std::string m_content;  //!< content of the publication
    std::string m_userInfo; //!< user info associated with action (since MQTT v.5)
    uint8_t m_qos;          //!< quality of service (for defitinion of values see MQTT doc)
    bool m_retain;          //!< wheter or not the content should be retained (= kept as current)
};

#endif // DATAGRAMMQTT_HPP