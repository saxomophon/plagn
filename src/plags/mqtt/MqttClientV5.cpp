/**
 * -------------------------------------------------------------------------------------------------
 * @file MqttClientV5.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements the MqttClientV5 class
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

// self include
#include "MqttClientV5.hpp"

using namespace std;

MqttClientV5::MqttClientV5(const Plag & parent, const string & brokerIP, unsigned int brokerPort,
                           const string & clientId, uint8_t defaultQoS, const string & userName,
                           const string & userPass, unsigned int keepAliveInterval,
                           bool cleanSessions, const string & willTopic, const string willMessage,
                           const vector<std::pair<string, uint8_t>> & defaultSubscriptions) :
    MqttClient(parent, brokerIP, brokerPort, clientId, defaultQoS, userName, userPass,
               keepAliveInterval, cleanSessions, willTopic, willMessage, defaultSubscriptions, 5)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief reads properties from prepared string containing only the properties
 * 
 * @param content entire properties string, minus properties length indicator bytes
 * @return map<MqttPropertyType, string> 
 */
map<MqttPropertyType, string> MqttClientV5::readProperties(const std::string & content = 0) const
{
    map<MqttPropertyType, string> properties;
    string remainingProperties = content;
    string extracted;
    size_t extractedLength = 0;
    do
    {
        uint8_t type = remainingProperties.at(0);
        if (type == FORMAT
            || type == REQUEST_PROBLEM_INFO
            || type == REQUEST_RESPONSE_INFO
            || type == QOS_MAX
            || type == RETAIN_AVAILABLE
            || type == WILDCARD_SUB_AVAILABLE
            || type == SUBSCRIPTION_ID_AVAILABLE
            || type == SHARED_SUBSCRIPTION_AVAILABLE)
        {
            extracted = to_string(static_cast<int>(remainingProperties.at(1)));
            extractedLength = 2;
        }
        else if (type == KEEP_ALIVE
                 || type == RECEIVE_MAX
                 || type == TOPIC_ALIAS_MAX
                 || type == TOPIC_ALIAS
                 || type == RECEIVE_MAX)
        {
            unsigned int number = 0;
            number = static_cast<int>(remainingProperties.at(1)) << 8;
            number |= static_cast<int>(remainingProperties.at(2));
            extracted = to_string(number);
            extractedLength = 3;
        }
        else if (type == EXPIRES
                 || type == SESSION_EXPIRE
                 || type == WILL_DELAY
                 || type == PACKET_SIZE_MAX)
        {
            unsigned int number = 0;
            number = static_cast<int>(remainingProperties.at(1)) << 24;
            number |= static_cast<int>(remainingProperties.at(2)) << 16;
            number |= static_cast<int>(remainingProperties.at(3)) << 8;
            number |= static_cast<int>(remainingProperties.at(2));
            extracted = to_string(number);
            extractedLength = 5;
        }
        else if (type == SUBSCRIPTION_ID)
        {
            extractedLength = 1;
            uint8_t offset;
            unsigned int number = readMqttVarInt(remainingProperties, offset, 1);
            extractedLength += offset;
            extracted = to_string(number);
        }
        else if (type == CORRELATION_DATA)
        {
            extracted = remainingProperties.substr(1);
            extractedLength = remainingProperties.size();
        }
        else if (type == USER_PROPERTY)
        {
            uint16_t length = static_cast<uint16_t>(remainingProperties.at(1)) << 8;
            length |= static_cast<uint16_t>(remainingProperties.at(2));
            uint16_t length2 = static_cast<uint16_t>(remainingProperties.at(1 + length)) << 8;
            length2 |= static_cast<uint16_t>(remainingProperties.at(2 + length));
            extracted = properties.count(USER_PROPERTY) > 0 ? properties.at(USER_PROPERTY) : "";
            extracted += remainingProperties.substr(1, length + length2 + 4);
            extractedLength = length + length2 + 5;
        }
        else
        {
            extracted = extractMqttString(remainingProperties, 1);
            extractedLength = extracted.size() + 3;
        }

        properties.insert_or_assign(static_cast<MqttPropertyType>(type), extracted);
        remainingProperties = remainingProperties.substr(extractedLength);
    } while (remainingProperties.size() > 0);
    return properties;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The User Properties of MQTT consists of key-value pairs in MQTT string, lets make 'em a
 * good ol' regular map
 *
 * @param properties all the properties of an MQTT message
 * @return map<string, string> 
 */
map<string, string> MqttClientV5::convertUserPropertiesToMap(const map<MqttPropertyType,
                                                                       string> & properties) const
{
    map<string, string> userProps;
    string userPropsAsStr = properties.at(USER_PROPERTY);
    string propKey;
    string propValue;
    size_t currentPos = 0;
    do
    {
        propKey = extractMqttString(userPropsAsStr, currentPos);
        currentPos += propKey.size() + 2;
        if (currentPos >= userPropsAsStr.size()) throw std::invalid_argument("No key-value pair!");
        propValue = extractMqttString(userPropsAsStr, currentPos);
        currentPos += propValue.size() + 2;
        userProps.insert_or_assign(propKey, propValue);
    } while (currentPos < userPropsAsStr.size());
    return userProps;
}
/**
 *-------------------------------------------------------------------------------------------------
 * @brief generates a string for one property only
 *
 * @param type types of the property
 * @param data data to the @p type of property
 * @return @p type and @p data in the correct format
 * @remark intended use is to fill a properties string using this method, and then add it to the
 * payload by adding the size as an MQTT varInt and then the properties string
 * @sa MqttInterface::makeMqttVarInt()
 */
string MqttClientV5::makeProperty(MqttPropertyType type, const DataType & data) const try
{
    string props;
    props += type;
    if (type == FORMAT
        || type == REQUEST_PROBLEM_INFO
        || type == REQUEST_RESPONSE_INFO
        || type == QOS_MAX
        || type == RETAIN_AVAILABLE
        || type == WILDCARD_SUB_AVAILABLE
        || type == SUBSCRIPTION_ID_AVAILABLE
        || type == SHARED_SUBSCRIPTION_AVAILABLE)
    {
        props += static_cast<char>(convertDataTypeToInt(data) & 0xFF);
    }
    else if (type == KEEP_ALIVE
             || type == RECEIVE_MAX
             || type == TOPIC_ALIAS_MAX
             || type == TOPIC_ALIAS
             || type == RECEIVE_MAX)
    {
        props += static_cast<char>((convertDataTypeToUint(data) & 0xFF00) >> 8);
        props += static_cast<char>(convertDataTypeToUint(data) & 0xFF);
    }
    else if (type == EXPIRES
             || type == SESSION_EXPIRE
             || type == WILL_DELAY
             || type == PACKET_SIZE_MAX)
    {
        unsigned int number = convertDataTypeToUint(data);
        props += static_cast<char>((number & 0xFF000000) >> 24);
        props += static_cast<char>((number & 0xFF0000) >> 16);
        props += static_cast<char>((number & 0xFF00) >> 8);
        props += static_cast<char>(number & 0xFF);
    }
    else if (type == SUBSCRIPTION_ID)
    {
        unsigned int number = convertDataTypeToUint(data) & 0x0FFFFFFF;
        props += makeMqttVarInt(number);
    }
    else if (type == CORRELATION_DATA)
    {
        props += convertDataTypeToString(data);
    }
    else if (type == USER_PROPERTY)
    {
        map<string, string> userProps = convertDataTypeToMap(data);
        bool isFirst = true;
        for (const pair<string, string> & item : userProps)
        {
            if (isFirst)
            {
                isFirst = false;
            }
            else
            {
                props += type;
            }
            props += makeMqttString(item.first);
            props += makeMqttString(item.second);
        }
    }
    else
    {
        props += makeMqttString(convertDataTypeToString(data));
    }
    return props;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::makeProperty()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief convenience function to create the data for a connection message for the broker
 *
 * @return string the version specific connect message using config data
 */
string MqttClientV5::createConnectMessage() try
{
    string connectMsg;
    connectMsg += makeMqttString("MQTT");
    connectMsg += static_cast<char>(m_protocolVersion); // v. 5
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
    if (m_cleanSessions) flags |= 0x02;  // := clean start(v. 5.0)
    // 0x01:= reserved last bit: always zero
    connectMsg += flags;
    // add keepalive
    connectMsg += static_cast<char>((m_keepAliveInterval & 0xFF00) >> 8);
    connectMsg += static_cast<char>(m_keepAliveInterval & 0x00FF);

    vector<MqttPropertyType> types;
    vector<string> dataToTypes;
    string dataToType;
    string properties;

    properties = makeProperty(SESSION_EXPIRE, m_sessionExpire);

    // receive maximum will not be set, as use-case is unclear
    // maximum packer size will not be set, as use-case is unclear
    // topic alias maximum will not be set, as implementation does not seem worth the gain

    // always ask for info in connack message
    // unlikely to result in a benefit, when made user-settable
    int on = 1;
    properties += makeProperty(REQUEST_RESPONSE_INFO, on);

    // always ask for problem description in connack message
    // unlikely to result in a benefit, when made user-settable
    properties += makeProperty(REQUEST_PROBLEM_INFO, on);

    // no user-property will be set, as configuration is unclear
    // no authentication method will be set, as none is implemented yet
    // no authentication data will be set, see above

    // for now, no authentication method is correctly implemented, so no authentication flags
    connectMsg += makeMqttVarInt(properties.size()) + properties;

    // add client ID
    connectMsg += makeMqttString(m_clientId);

    // prepare will properties
    if (m_willMessage != "")
    {
        types.clear();
        dataToTypes.clear();
        dataToType = "";

        // will delay
        properties = makeProperty(WILL_DELAY, m_willDelay);

        // will format
        properties += makeProperty(FORMAT, static_cast<int>(m_willIsText));

        // will content type will not be set, as use-case is unclear
        // response topic will not be set, as the request/response mechanism is not yet supported
        // correlation data will not be set, as the request/response mechanism is not yet supported
        // response topic will not be set, as the request/response mechanism is not yet supported
        // user property will not be set, as configuration seems too complicated

        connectMsg += makeMqttVarInt(properties.size()) + properties;
        connectMsg += makeMqttString(m_willTopic);
        connectMsg += makeMqttString(m_willMessage);
    }

    if (m_userName != "") connectMsg += makeMqttString(m_userName);
    if (m_userPass != "") connectMsg += m_userPass;

    prepareFixedHeader(CONNECT, 0, connectMsg);

    cout << "Connect with: " << getBinStringAsAsciiHex(connectMsg) << endl;

    return connectMsg;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::createConnectMessage()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief parser for connect, as demanded by base class. Does nothing however, as this is a client.
 *
 * @param content ignored content of the CONNECT request
 */
void MqttClientV5::parseConnect(const string & content) try
{
    cout << "This is a client, and therefore cannot be connected to!" << endl;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::parseConnect()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses a connection acknowledgment (conn ack) message and acts upon it
 *
 * @param content the complete message
 */
void MqttClientV5::parseConnAck(const string & content) try
{
    if (content.size() < 2) return;
    if (content.at(0) != static_cast<char>(CONNACK << 4))
    {
        cerr << "Called MqttClientV5::parseConnAck from non-CONNACK message!" << endl;
        return;
    }
    bool sessionReconnect = (content.at(2) == static_cast<char>(0x01));
    if (sessionReconnect) cout << "Broker revived previous session" << endl;

    m_brokerConnected = static_cast<unsigned char>(content.at(3)) == 0x00;

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
    uint8_t offset;
    unsigned int size = readMqttVarInt(content, offset, 4);
    map<MqttPropertyType, string> properties = readProperties(content.substr(4 + offset, size));

    // for debugging only (TODO: remove)
    cout << "PROPERTIES IN CONNACK \t";
    for (const pair<MqttPropertyType, string> & property : properties)
    {
        cout << (property.first < 0x10 ? "0" : "") << std::hex << static_cast<int>(property.first);
        cout << " : " << property.second << "\t";
    }
    cout << endl;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::parseConnAck()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief parses a disconnection (DISCONNECT) message and acts upon it
 * 
 * @param content 
 */
void MqttClientV5::parseDisconnect(const std::string & content) try
{
    uint8_t reason = static_cast<uint8_t>(content.at(0));
    if (reason & 0x80) cout << "Disconnect because of error: " << std::hex << static_cast<int>(reason) << endl;

    uint8_t offset;
    unsigned int size = readMqttVarInt(content, offset, 1);
    map<MqttPropertyType, string> properties = readProperties(content.substr(1 + offset, size));

    m_brokerConnected = false;
    m_transportLayer->disconnect();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::parseDisconnect()";
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
void MqttClientV5::parsePublish(uint8_t firstByte, string & content) try
{
    if (((firstByte & 0xF0) >> 4) != PUBLISH)
    {
        cerr << "Called MqttClientV5::parsePublish from non-PUBLISH message!" << endl;
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

    uint8_t offset;
    unsigned int size = readMqttVarInt(content, offset);
    map<MqttPropertyType, string> properties = readProperties(content.substr(offset, size));
    content = content.substr(offset + size);

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
    errorMsg += "\nSomething happened in MqttClientV5::parsePublish()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses a PUBACK (publish acknowledgment) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV5::parsePubAck(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));

    uint8_t reason = static_cast<uint8_t>(content.at(4));
    if (reason & 0x80) cout << "Error in PUBACK: " << std::hex << reason << endl;

    uint8_t offset;
    unsigned int size = readMqttVarInt(content, offset, 5);
    map<MqttPropertyType, string> properties = readProperties(content.substr(5 + offset, size));

    removeNonAcknowledgedData(identifier);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::parsePubAck()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses a PUBREC (publish received) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV5::parsePubRec(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));
    string identifierAsStr = content.substr(2, 2);

    uint8_t reason = static_cast<uint8_t>(content.at(4));
    if (reason & 0x80) cout << "Error in PUBREC: " << std::hex << reason << endl;

    uint8_t offset;
    unsigned int size = readMqttVarInt(content, offset, 5);
    map<MqttPropertyType, string> properties = readProperties(content.substr(5 + offset, size));

    removeNonAcknowledgedData(identifier);

    tranmitPubRel(identifierAsStr);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::parsePubRec()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses a PUBREL (publish release) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV5::parsePubRel(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));
    string identifierAsStr = content.substr(2, 2);

    uint8_t reason = static_cast<uint8_t>(content.at(4));
    if (reason & 0x80) cout << "Error in PUBREL: " << std::hex << reason << endl;

    uint8_t offset;
    unsigned int size = readMqttVarInt(content, offset, 5);
    map<MqttPropertyType, string> properties = readProperties(content.substr(5 + offset, size));

    removeNonAcknowledgedData(identifier);

    transmitPubComp(identifierAsStr);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::parsePubRel()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parases a PUBCOMP (publish complete) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV5::parsePubComp(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));

    uint8_t reason = static_cast<uint8_t>(content.at(4));
    if (reason & 0x80) cout << "Error in PUBCOMP: " << std::hex << reason << endl;

    uint8_t offset;
    unsigned int size = readMqttVarInt(content, offset, 5);
    map<MqttPropertyType, string> properties = readProperties(content.substr(5 + offset, size));

    removeNonAcknowledgedData(identifier);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::parsePubComp()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses a SUBACK (subscription acknowledgment) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV5::parseSubAck(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));

    removeNonAcknowledgedData(identifier);

    uint8_t offset;
    unsigned int size = readMqttVarInt(content, offset, 4);
    map<MqttPropertyType, string> properties = readProperties(content.substr(4 + offset, size));

    string flags = content.substr(4 + offset + size);

    for (const char & flag : flags)
    {
        if (flag >= 0x80)
        {
            //TODO: implement fail-behaviour
            cout << "Failed subscription: " << to_string(identifier) << endl;
        }
        else
        {
            uint8_t qos = flag & 0x03;
            cout << "Max. qos for subscription: " << to_string(qos) << endl;
        }
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::parseSubAck()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses an UNSUBACK (unsubscribe acknowledgment) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV5::parseUnsubAck(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));

    removeNonAcknowledgedData(identifier);

    uint8_t offset;
    unsigned int size = readMqttVarInt(content, offset, 4);
    map<MqttPropertyType, string> properties = readProperties(content.substr(4 + offset, size));

    string flags = content.substr(4 + offset + size);

    for (const char & flag : flags)
    {
        if (flag >= 0x80)
        {
            //TODO: implement fail-behaviour
            cout << "Failed subscription: " << to_string(identifier) << endl;
        }
        else
        {
            uint8_t qos = flag & 0x03;
            cout << "Max. qos for subscription: " << to_string(qos) << endl;
        }
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::parseUnsubAck()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief parses an AUTH (unsubscribe acknowledgment) MQTT message
 *
 * @param content the complete message
 */
void MqttClientV5::parseAuth(const string & content) try
{
    if (content.size() < 4) return;

    uint16_t identifier = static_cast<uint16_t>(content.at(2)) << 8;
    identifier |= static_cast<uint16_t>(content.at(3));

    removeNonAcknowledgedData(identifier);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::parseAuth()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a message of type AUTH
 * @details AUTH means to authenticate this client at the broker (server).
 * @param reauthenticate whether or not this is an intial authentication (false), or not (true)
 */
void MqttClientV5::transmitAuth(bool reauthenticate) try
{
    string data;
    string properties;
    data += reauthenticate ? '\x19' : '\x18';

    properties = makeProperty(AUTH_METHOD, m_authenticationType);

    // TODO: generate authentication data depending on method and iteration of method
    properties += makeProperty(AUTH_DATA, m_authenticationData);

    properties += makeProperty(REASON, m_authenticationType);

    data += makeMqttVarInt(properties.size()) + properties;

    prepareFixedHeader(AUTH, 0, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::transmitPublish()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief creates and sends a message of type DISCONNECT
 * 
 */
void MqttClientV5::transmitDisconnect() try
{
    string data;

    // add reason
    data += m_willMessage != "" ? "\x04" : "\x00";

    // add zero properties (for now)
    data += "\x00";

    prepareFixedHeader(DISCONNECT, 0, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();

    m_brokerConnected = false;
    m_transportLayer->disconnect();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::transmitDisconnect()";
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
void MqttClientV5::transmitPublish(const string & topic, const string & content, uint8_t flags) try
{
    string data = makeMqttString(topic);

    // identifier is only present on qos larger 0
    uint16_t identifier = 0;
    if (((flags & 0x03) >> 1) > 0)
    {
        identifier = this->generateIdentifier();

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
    errorMsg += "\nSomething happened in MqttClientV5::transmitPublish()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a PUBACK (publish acknowledge) message
 * @details these need to be sent, when this receives a PUBLISH message with qos == 1
 * @param identifier identifier of the received PUBLISH message
 * @param reasonCode MQTT v.5 adds a reason code, to convey more info. this is that
 */
void MqttClientV5::transmitPubAck(const string & identifier, char reasonCode) try
{
    string data = identifier;

    if (m_protocolVersion == 5 && reasonCode != 0)
    {
        data += reasonCode;
        data += '\x00';     // we do not wish to add properties for debugging
    }

    prepareFixedHeader(PUBACK, 0, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::transmitPubAck()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a PUBREC (publish received) message
 * @details these need to be sent, when this receives a PUBLISH message with qos == 2
 * @param identifier identifier of the received PUBLISH message
 * @param reasonCode MQTT v.5 adds a reason code, to convey more info. this is that
 */
void MqttClientV5::transmitPubRec(const string & identifier, char reasonCode) try
{
    string data = identifier;

    if (m_protocolVersion == 5 && reasonCode != 0)
    {
        data += reasonCode;
        data += '\x00';     // we do not wish to add properties for debugging
    }

    prepareFixedHeader(PUBREC, 0, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::transmitPubRec()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a PUBREL (publish release) message
 * @details these need to be sent, when this receives a PUBREC message
 * @param identifier identifier of the received PUBREC message
 * @param reasonCode MQTT v.5 adds a reason code, to convey more info. this is that
 */
void MqttClientV5::tranmitPubRel(const string & identifier, char reasonCode) try
{
    string data = identifier;

    if (m_protocolVersion == 5 && reasonCode != 0)
    {
        data += reasonCode;
        data += '\x00';     // we do not wish to add properties for debugging
    }

    prepareFixedHeader(PUBREL, 2, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::tranmitPubRel()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a PUBCOMP (publish complete) message
 * @details these need to be sent, when this receives a PUBREL message
 * @param identifier identifier of the received PUBREL message
 * @param reasonCode MQTT v.5 adds a reason code, to convey more info. this is that
 */
void MqttClientV5::transmitPubComp(const string & identifier, char reasonCode) try
{
    string data = identifier;

    if (m_protocolVersion == 5 && reasonCode != 0)
    {
        data += reasonCode;
        data += '\x00';     // we do not wish to add properties for debugging
    }

    prepareFixedHeader(PUBCOMP, 0, data);

    m_transportLayer->transmit(data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::transmitPubComp()";
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
void MqttClientV5::transmitSubscribe(const string & topic, uint8_t qos) try
{
    string data = "";

    uint16_t identifier = generateIdentifier();

    data += static_cast<char>((identifier & 0xFF00) >> 8);
    data += static_cast<char>(identifier & 0x00FF);

    // add zero properties (for now)
    data += "\x00";

    data += makeMqttString(topic);
    data += static_cast<char>(qos & 0x03);

    prepareFixedHeader(SUBSCRIBE, 2, data);

    m_transportLayer->transmit(data);

    addNonAcknowledgedData(identifier, data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::transmitSubscribe()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief creates and sends a UNSUBSCRIBE message
 *
 * @param topic name of the subscribed channel (tree structure) to unsubscribe from
 */
void MqttClientV5::transmitUnsubscribe(const string & topic) try
{
    string data = "";

    uint16_t identifier = generateIdentifier();

    data += static_cast<char>((identifier & 0xFF00) >> 8);
    data += static_cast<char>(identifier & 0x00FF);

    if (m_protocolVersion < 5)
    {
        data += makeMqttString(topic);
    }
    else if (m_protocolVersion == 5)
    {
        //TODO: create writeProperty to add content as such
    }

    prepareFixedHeader(UNSUBSCRIBE, 2, data);

    m_transportLayer->transmit(data);

    addNonAcknowledgedData(identifier, data);

    m_lastTimeOfSent = std::chrono::steady_clock::now();
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttClientV5::transmitUnsubscribe()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}