/**
 *-------------------------------------------------------------------------------------------------
 * @file PlagMqtt.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the PlagMqtt class
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

#ifndef PLAGMQTT_HPP
#define PLAGMQTT_HPP

// std includes

// own includes
#include "MqttInterface.hpp"
#include "Plag.hpp"
#include "TcpClient.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The PlagMqtt class is a Plag to interact with the MQTT protocol
 * 
 */
class PlagMqtt : public Plag
{
public:
    PlagMqtt(const boost::property_tree::ptree & propTree,
            const std::string & name, const uint64_t & id);
    ~PlagMqtt();

    virtual void readConfig();

    virtual void init();

    virtual bool loopWork();

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram);

private:

private:
    // config parameters
    uint8_t m_mqttVersion;              //!< version of the MQTT protocol to use
    std::string m_brokerIP;             //!< ip the endpoint should bind to
    uint16_t m_port;                    //!< port the endpoint should bind to    
    uint16_t m_timeTimeout;             //!< time limit for communcation with device in ms
    std::string m_userName;             //!< the client username provided to the Broker
    std::string m_userPass;             //!< password associated with m_userName
    std::string m_authenticationType;   //!< method to use for authentication
    std::string m_authenticationData;   //!< data to use with authentication
    unsigned int m_keepAliveInterval;   //!< keepaliveinterval in s
    bool m_cleanSessions;               //!< whether or not to always start clean sessions
    unsigned int m_sessionExpire;       //!< when a session should expire (0 = on disconnect, UINT_MAX = never)
    std::vector<std::pair<std::string, uint8_t>> m_defaultSubscriptions; //!< default subscriptions, the pair is organized as <topicName, qos>

    // worker members
    std::shared_ptr<MqttInterface> m_interface; //!< interface to MQTT
};

#endif // PLAGMQTT_HPP
