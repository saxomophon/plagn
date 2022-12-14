/**
 * -------------------------------------------------------------------------------------------------
 * @file MqttClientV4.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements the MqttClientV4 class
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

// self include
#include "MqttClientV4.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Mqtt Client V 4 by calling base class constructor with fixed version
 *
 * @param parent reference to parent Plag
 * @param brokerIP ip of the broker as a string
 * @param brokerPort port of the broker to use
 * @param clientId this' id as presented to the broker
 * @param defaultQoS a default QoS level
 * @param userName username to supply to broker
 * @param userPass password to supply alongside username
 * @param keepAliveInterval keepalive interval in ms
 * @param cleanSessions flag, whether to start on a new session (true), or not (false)
 * @param willTopic topic of the testament
 * @param willMessage content of the testament
 * @param defaultSubscriptions default list of subsrciptions
 * @sa MqttClient::MqttClient()  
 */
MqttClientV4::MqttClientV4(const Plag & parent, const string & brokerIP, unsigned int brokerPort,
                           const string & clientId, uint8_t defaultQoS, const string & userName,
                           const string & userPass, unsigned int keepAliveInterval,
                           bool cleanSessions, const string & willTopic, const string willMessage,
                           const vector<std::pair<string, uint8_t>> & defaultSubscriptions) :
    MqttClient(parent, brokerIP, brokerPort, clientId, defaultQoS, userName, userPass,
               keepAliveInterval, cleanSessions, willTopic, willMessage, defaultSubscriptions, 4)
{
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief convenience function to create the data for a connection message for the broker
 *
 * @return string the version specific connect message using config data
 */
string MqttClientV4::createConnectMessage() try
{
    string connectMsg;
    connectMsg += makeMqttString("MQTT");
    connectMsg += static_cast<char>(m_protocolVersion); // v. 3.1.1
    uint8_t flags = 0;
    if (m_userName != "")
    {
        flags |= 0x80; // use user name
        if (m_userPass != "") flags |= 0x40; // use password
    }
    // will qos
    if (m_qualityOfService == 2) flags |= 0x10;
    else if (m_qualityOfService == 1) flags |= 0x08;
    if (m_willMessage != "")
    {
        flags |= 0x04; // will message will be added
        flags |= 0x20; // will retain (= inform new subscribers, that <this> disconnected abnormally)
    }
    if (m_cleanSessions) flags |= 0x02;  // := clean session(v. 3.1.1) or clean start(v. 5.0)
    // 0x01:= reserved last bit: always zero
    connectMsg += flags;
    // add keepalive
    connectMsg += static_cast<char>((m_keepAliveInterval & 0xFF00) >> 8);
    connectMsg += static_cast<char>(m_keepAliveInterval & 0x00FF);

    // add client ID
    connectMsg += makeMqttString(m_clientId);
    // add will topic
    if (m_willTopic != "") connectMsg += makeMqttString(m_willTopic);
    // add will message
    if (m_willMessage != "") connectMsg += makeMqttString(m_willMessage);
    if (m_userName != "")
    {
        // add user name
        connectMsg += makeMqttString(m_userName);
        if (m_userPass != "")
        {
            // add user password
            connectMsg += makeMqttString(m_userPass);
        }
    }

    prepareFixedHeader(CONNECT, 0, connectMsg);
    return connectMsg;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::createConnectMessage()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief parser for connect, as demanded by base class. Does nothing however, as this is a client.
 * 
 * @param content ignored content of the CONNECT request
 */
void MqttClientV4::parseConnect(const string & content) try
{
    cout << "This is a client, and therefore cannot be connected to!" << endl;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parseConnect()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses a connection acknowledgment (conn ack) message and acts upon it
 *
 * @param content the complete message
 */
void MqttClientV4::parseConnAck(const string & content) try
{
    if (content.size() < 2) return;
    if (content.at(0) != static_cast<char>(CONNACK << 4))
    {
        cerr << "Called MqttClientV4::parseConnAck from non-CONNACK message!" << endl;
        cerr << "CONNACK message was: " << getBinStringAsAsciiHex(content) << endl;
        return;
    }
    if (content.at(1) != char(0x02))
    {
        cout << "Unknown CONNACK format. Will continue to parse." << endl;
        cout << "CONNACK message was: " << getBinStringAsAsciiHex(content) << endl;
    }
    m_brokerConnected = (content.at(2) == static_cast<char>(0x01));
    string ackMessage;
    switch (static_cast<unsigned char>(content.at(3)))
    {
    case 0:
        cout << "Connected to broker!" << endl;
        break;
    case 1:
        cout << "Protocol version refused! (v.3)" << endl;
        break;
    case 2:
        cerr << "Identifier rejected! (v.3)" << endl;
        break;
    case 3:
        cout << "Service unavailable! (v.3)" << endl;
        break;
    case 4:
        cerr << "Bad user credentials! (v.3)" << endl;
        break;
    case 5:
        cout << "This client is prohibited! (v.3)" << endl;
        break;
    case 0x80:
        cout << "Error not specified by broker! (v.5)" << endl;
        break;
    case 0x81:
        cout << "Malformed packet! (v.5)" << endl;
        break;
    case 0x82:
        cout << "Protocol error! (v.5)" << endl;
        break;
    case 0x83:
        cout << "Broker error! (v.5)" << endl;
        break;
    case 0x84:
        cout << "Unsupported protocol version! (v.5)" << endl;
        break;
    case 0x85:
        cerr << "Invalid client ID! (v.5)" << endl;
        break;
    case 0x86:
        cerr << "Bad user credentials! (v.5)" << endl;
        break;
    case 0x87:
        cout << "Not authorized! (v.5)" << endl;
        break;
    case 0x88:
        cout << "Server unavailable! (v.5)" << endl;
        break;
    case 0x89:
        cout << "Server busy! (v.5)" << endl;
        break;
    case 0x8A:
        cerr << "Banned! (v.5)" << endl;
        break;
    case 0x8C:
        cout << "Bad authentication method! (v.5)" << endl;
        break;
    case 0x90:
        cout << "Topic name invalid! (v.5)" << endl;
        break;
    case 0x95:
        cout << "Packet too large! (v.5)" << endl;
        break;
    case 0x97:
        cout << "Quota exceeded! (v.5)" << endl;
        break;
    case 0x99:
        cout << "Payload format invalid! (v.5)" << endl;
        break;
    case 0x9A:
        cout << "Retain not supported! (v.5)" << endl;
        break;
    case 0x9B:
        cout << "QoS not supported! (v.5)" << endl;
        break;
    case 0x9C:
        cerr << "Use another server! (v.5)" << endl;
        break;
    case 0x9D:
        cerr << "Server moved! (v.5)" << endl;
        break;
    case 0x9F:
        cout << "Connection rate exceeded! (v.5)" << endl;
        break;
    default:
        cout << "Unknown error!" << endl;
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parseConnAck()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief parses a disconnection (DISCONNECT) message and acts upon it
 *
 * @param content
 */
void MqttClientV4::parseDisconnect(const std::string & content) try
{
    if (content.size() > 0) cout << "Protocol error, as disconnect should be empty" << endl;
    m_brokerConnected = false;
    m_transportLayer->disconnect();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parseDisconnect()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses the variable header and payload of a PUBLISH MQTT message
 *
 * @param firstByte the first byte of the fixed header of the MQTT message
 * @param content the variable header and payload of the MQTT message
 */
void MqttClientV4::parsePublish(uint8_t firstByte, string & content) try
{
    if (((firstByte & 0xF0) >> 4) != PUBLISH)
    {
        cerr << "Called MqttClientV4::parsePublish from non-PUBLISH message!" << endl;
        return;
    }
    // target to write to
    string action = "publish";
    // read flags
    uint8_t flags = firstByte & 0x0F;
    bool mightBeDuplicate = flags & 0x08;
    uint8_t qos = (flags & 0x06) >> 1;
    bool retain = flags & 0x01;

    string topic = extractMqttString(content);
    string identifier;
    if (qos > 0)
    {
        identifier = content.substr(0, 2);
        content = content.substr(2);
    }
    string msgContent = "";

    msgContent = content;

    if (qos == 1)
    {
        transmitPubAck(identifier);
    }
    else if (qos == 2)
    {
        transmitPubRec(identifier);
    }
    shared_ptr<DatagramMqtt> datagram(new DatagramMqtt(m_parent.getName(), action,
                                                       topic, msgContent, qos, retain));
    m_incomingBuffer.push_back(datagram);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parsePublish()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses a PUBACK (publish acknowledgment) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV4::parsePubAck(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));

    removeNonAcknowledgedData(identifier);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parsePubAck()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses a PUBREC (publish received) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV4::parsePubRec(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));
    string identifierAsStr = content.substr(2, 2);

    removeNonAcknowledgedData(identifier);

    tranmitPubRel(identifierAsStr);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parsePubRec()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses a PUBREL (publish release) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV4::parsePubRel(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));
    string identifierAsStr = content.substr(2, 2);

    removeNonAcknowledgedData(identifier);

    transmitPubComp(identifierAsStr);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parsePubRel()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parases a PUBCOMP (publish complete) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV4::parsePubComp(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));

    removeNonAcknowledgedData(identifier);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parsePubComp()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses a SUBACK (subscription acknowledgment) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV4::parseSubAck(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));

    removeNonAcknowledgedData(identifier);

    uint8_t flags = content.at(3);

    if (flags >= 0x80)
    {
        //TODO: implement to handle failed subscriptions
        cout << "Failed subscription: " << to_string(identifier) << endl;
    }
    else
    {
        uint8_t qos = flags & 0x03;
        cout << "Max. qos for subscription: " << to_string(qos) << endl;
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parseSubAck()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses an UNSUBACK (unsubscribe acknowledgment) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV4::parseUnsubAck(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));

    removeNonAcknowledgedData(identifier);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parseUnsubAck()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief parser for AUTH (authentication), as demanded by base class. Does nothing however, as this
 * is a version 4 client.
 *
 * @param content 
 */
void MqttClientV4::parseAuth(const string & content) try
{
    cout << "AUTH is not a part of the MQTT v3.1.1 spec. Adapt version!" << endl;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::parseAuth()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief Exists, as demanded by base-class. Does nothing as this is a version 4 client.
 * @details AUTH means to authenticate this client at the broker (server).
 * @param reauthenticate whether or not this is an intial authentication (false), or not (true)
 */
void MqttClientV4::transmitAuth(bool reauthenticate) try
{
    cout << "Authentication message not part of the MQTT v3.1.1 spec" << endl;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::transmitPublish()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief creates and sends a message of type DISCONNECT
 *
 */
void MqttClientV4::transmitDisconnect() try
{
    string data = "";

    prepareFixedHeader(DISCONNECT, 0, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();

    m_brokerConnected = false;
    m_transportLayer->disconnect();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::transmitDisconnect()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a message of type PUBLISH
 * @details PUBLISH means to distribute data ( @p content ) among the subscribers of @p topic .
 * @param topic the topic under which the @p content is published
 * @param content the content to publish (may be values, may be text)
 * @param flags the 4-bit flags (qos, retain, and dup). dup is currently unused and should not be set by the user
 */
void MqttClientV4::transmitPublish(const string & topic, const string & content, uint8_t flags) try
{
    string data = makeMqttString(topic);

    // identifier is only present on qos larger 0
    uint16_t identifier = 0;
    if (((flags & 0x06) >> 1) > 0)
    {
        identifier = generateIdentifier();

        data += static_cast<char>((identifier & 0xFF00) >> 8);
        data += static_cast<char>(identifier & 0x00FF);
    }

    data += content;

    prepareFixedHeader(PUBLISH, flags, data);

    m_transportLayer->transmit(data);

    if (identifier != 0) addNonAcknowledgedData(identifier, data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::transmitPublish()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a PUBACK (publish acknowledge) message
 * @details these need to be sent, when this receives a PUBLISH message with qos == 1
 * @param identifier identifier of the received PUBLISH message
 * @param reasonCode MQTT v.5 adds a reason code, to convey more info. this is ignored
 */
void MqttClientV4::transmitPubAck(const string & identifier, char reasonCode) try
{
    string data = identifier;

    prepareFixedHeader(PUBACK, 0, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::transmitPubAck()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a PUBREC (publish received) message
 * @details these need to be sent, when this receives a PUBLISH message with qos == 2
 * @param identifier identifier of the received PUBLISH message
 * @param reasonCode MQTT v.5 adds a reason code, to convey more info. this is ignored
 */
void MqttClientV4::transmitPubRec(const string & identifier, char reasonCode) try
{
    string data = identifier;

    prepareFixedHeader(PUBREC, 0, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::transmitPubRec()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a PUBREL (publish release) message
 * @details these need to be sent, when this receives a PUBREC message
 * @param identifier identifier of the received PUBREC message
 * @param reasonCode MQTT v.5 adds a reason code, to convey more info. this is ignored
 */
void MqttClientV4::tranmitPubRel(const string & identifier, char reasonCode) try
{
    string data = identifier;

    prepareFixedHeader(PUBREL, 2, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::tranmitPubRel()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a PUBCOMP (publish complete) message
 * @details these need to be sent, when this receives a PUBREL message
 * @param identifier identifier of the received PUBREL message
 * @param reasonCode MQTT v.5 adds a reason code, to convey more info. this is ignored
 */
void MqttClientV4::transmitPubComp(const string & identifier, char reasonCode) try
{
    string data = identifier;

    prepareFixedHeader(PUBCOMP, 0, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::transmitPubComp()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a SUBSCRIBE message
 *
 * @param topic name of the subscription channel (tree structure)
 * @param qos quality of service targeted for this subscription
 */
void MqttClientV4::transmitSubscribe(const string & topic, uint8_t qos) try
{
    string data = "";

    uint16_t identifier = generateIdentifier();

    data += static_cast<char>((identifier & 0xFF00) >> 8);
    data += static_cast<char>(identifier & 0x00FF);
    data += makeMqttString(topic);
    data += static_cast<char>(qos & 0x03);

    prepareFixedHeader(SUBSCRIBE, 2, data);

    cout << "Subscribe to " << topic << endl;

    m_transportLayer->transmit(data);

    addNonAcknowledgedData(identifier, data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::transmitSubscribe()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a UNSUBSCRIBE message
 *
 * @param topic name of the subscribed channel (tree structure) to unsubscribe from
 */
void MqttClientV4::transmitUnsubscribe(const string & topic) try
{
    string data = "";

    uint16_t identifier = generateIdentifier();

    data += static_cast<char>((identifier & 0xFF00) >> 8);
    data += static_cast<char>(identifier & 0x00FF);
    data += makeMqttString(topic);

    prepareFixedHeader(UNSUBSCRIBE, 2, data);

    m_transportLayer->transmit(data);

    addNonAcknowledgedData(identifier, data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV4::transmitUnsubscribe()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}