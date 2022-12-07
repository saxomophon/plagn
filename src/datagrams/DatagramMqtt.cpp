/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramMqtt.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors: 
 * @brief Implementation of the DatagramMqtt class
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

// self include
#include "DatagramMqtt.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Datagram:: Datagram object sets member values
 * 
 * @param sourcePlag the origin of this Datagram
 */
DatagramMqtt::DatagramMqtt(const string & sourcePlag) :
    Datagram(sourcePlag),
    m_action(""),
    m_topic(""),
    m_content(""),
    m_userInfo(""),
    m_qos(1),
    m_retain(false)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief overload to Construct a new Datagram:: Datagram object sets member values
 *
 * @param sourcePlag the origin of this Datagram
 */
DatagramMqtt::DatagramMqtt(const string & sourcePlag, const string & action, const string & topic,
                           const string & content, uint8_t qos, bool retain) :
    Datagram(sourcePlag),
    m_action(action),
    m_topic(topic),
    m_content(content),
    m_userInfo(""),
    m_qos(qos),
    m_retain(retain)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 * 
 * @return const string& member value
 */
const string & DatagramMqtt::getAction() const
{
    return m_action;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return const string& member value
 */
const string & DatagramMqtt::getTopic() const
{
    return m_topic;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return const string& member value
 */
const string & DatagramMqtt::getContent() const
{
    return m_content;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return const string& member value
 */
const string & DatagramMqtt::getUserInfo() const
{
    return m_userInfo;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return unsigned int member value
 */
unsigned int DatagramMqtt::getQoS() const
{
    return m_qos;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return const string& member value
 */
bool DatagramMqtt::getRetainFlag() const
{
    return m_retain;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to access data of Datagram under just one name
 *
 * @param key reference name of the value to access
 * @return DataType
 */
DataType DatagramMqtt::getData(const string & key) const try
{
    if (key == string("action"))
    {
        return getAction();
    }
    else if (key == string("topic"))
    {
        return getTopic();
    }
    else if (key == string("content"))
    {
        return getContent();
    }
    else if (key == string("userInfo"))
    {
        return getUserInfo();
    }
    else if (key == string("qos"))
    {
        return getQoS();
    }
    else if (key == string("retain"))
    {
        return static_cast<unsigned int>(getRetainFlag());
    }
    else // use base class implementation
    {
        return Datagram::getData(key);
    }
}
catch (std::invalid_argument & e)
{
    throw std::invalid_argument(string("In DatagramMqtt: ") + e.what());
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramMqtt::getData() ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to write data of Datagram under just one name
 *
 * @param key reference name of the value to override
 * @param value value to bet set for member accssible by @p key
 */
void DatagramMqtt::setData(const string & key, const DataType & value) try
{
    if (key == string("action"))
    {
        m_action = convertDataTypeToString(value);
    }
    else if (key == string("topic"))
    {
        m_topic = convertDataTypeToString(value);
    }
    else if (key == string("content"))
    {
        m_content = convertDataTypeToString(value);
    }
    else if (key == string("userInfo"))
    {
        m_userInfo = convertDataTypeToString(value);
    }
    else if (key == string("qos"))
    {
        m_qos = static_cast<uint8_t>(convertDataTypeToUint(value));
    }
    else if (key == string("retain"))
    {
        m_retain = convertDataTypeToBoolean(value);
    }
    else // use base class implementation
    {
        Datagram::setData(key, value);
    }
}
catch (std::invalid_argument & e)
{
    throw std::invalid_argument(string("In DatagramMqtt: ") + e.what());
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramMqtt::setData(): ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief creates a string representation of this Datagram
 * 
 * @return string 
 */
string DatagramMqtt::toString() const try
{
    string stringRepresentation = Datagram::toString();
    stringRepresentation += "{MQTT info: action: " + m_action;
    stringRepresentation += "; topic: " + m_topic;
    stringRepresentation += "; content: " + m_content;
    stringRepresentation += "; userInfo: " + m_userInfo;
    stringRepresentation += "; QoS: " + to_string(m_qos);
    stringRepresentation += string("; retain: ") + string(m_retain ? "true" : "false");
    stringRepresentation += "}";
    return stringRepresentation;
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramMqtt::toString(): ") + e.what());
}
