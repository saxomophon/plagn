/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramUdp.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors: 
 * @brief Implementation of the DatagramUdp class
 * @version 0.1
 * @date 2022-11-13
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
#include "DatagramUdp.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Datagram:: Datagram object sets member values
 * 
 * @param sourcePlag the origin of this Datagram
 */
DatagramUdp::DatagramUdp(const string & sourcePlag) :
    Datagram(sourcePlag),
    m_sender("this"),
    m_receiver("this"),
    m_port(0),
    m_payload("")
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief overload to Construct a new Datagram:: Datagram object sets member values
 *
 * @param sourcePlag the origin of this Datagram
 */
DatagramUdp::DatagramUdp(const string & sourcePlag, const string & sender, unsigned int port,
                         const string & payload) :
    Datagram(sourcePlag),
    m_sender(sender),
    m_receiver("this"),
    m_port(port),
    m_payload(payload)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 * 
 * @return const string& member value
 */
const string & DatagramUdp::getSender() const
{
    return m_sender;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return const string& member value
 */
const string & DatagramUdp::getReceiver() const
{
    return m_receiver;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return unsigned int member value
 */
unsigned int DatagramUdp::getPort() const
{
    return m_port;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return const string& member value
 */
const string & DatagramUdp::getPayload() const
{
    return m_payload;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to access data of Datagram under just one name
 *
 * @param key reference name of the value to access
 * @return DataType
 */
DataType DatagramUdp::getData(const string & key) const try
{
    if (key == string("sender"))
    {
        return m_sender;
    }
    else if (key == string("receiver"))
    {
        return m_receiver;
    }
    else if (key == string("port"))
    {
        return m_port;
    }
    else if (key == string("payload"))
    {
        return m_payload;
    }
    else // use base class implementation
    {
        return Datagram::getData(key);
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramUdp::getData() ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to write data of Datagram under just one name
 *
 * @param key reference name of the value to override
 * @param value value to bet set for member accssible by @p key
 */
void DatagramUdp::setData(const string & key, const DataType & value) try
{
    if (key == string("sender"))
    {
        m_sender = convertDataTypeToString(value);
    }
    else if (key == string("receiver"))
    {
        m_receiver = convertDataTypeToString(value);
    }
    else if (key == string("port"))
    {
        m_port = convertDataTypeToUint(value);
    }
    else if (key == string("payload"))
    {
        m_payload = convertDataTypeToString(value);
    }
    else // use base class implementation
    {
        Datagram::setData(key, value);
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramUdp::setData(): ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief creates a string representation of this Datagram
 * 
 * @return string 
 */
string DatagramUdp::toString() const try
{
    string stringRepresentation = Datagram::toString();
    stringRepresentation += "{UDP info: sender: " + m_sender;
    stringRepresentation += "; receiver: " + m_receiver;
    stringRepresentation += "; port: " + to_string(m_port);
    stringRepresentation += "; payload: " + m_payload;
    stringRepresentation += "}";
    return stringRepresentation;
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramUdp::toString(): ") + e.what());
}
