/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramHttpServer.cpp
 * @author Fabian Köslin (fabian@koeslin.info)
 * @contributors: 
 * @brief Implementation of the DatagramHttpServer class
 * @version 0.1
 * @date 2022-12-14
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
#include "DatagramHttpServer.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Datagram:: Datagram object sets member values
 * 
 * @param sourcePlag the origin of this Datagram
 */
DatagramHttpServer::DatagramHttpServer(const string & sourcePlag) :
    DatagramMap(sourcePlag)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief overload to Construct a new Datagram:: Datagram object sets member values
 *
 * @param sourcePlag the origin of this Datagram
 */
DatagramHttpServer::DatagramHttpServer(const string & sourcePlag,
    string reqId, map<string, DataType> map) :
    DatagramMap(sourcePlag, map),
    m_reqId(reqId)
{
}

std::string DatagramHttpServer::getReqId()
{
    return m_reqId;
}

void DatagramHttpServer::setReqId(std::string reqId)
{
    m_reqId = reqId;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to access data of Datagram under just one name
 *
 * @param key reference name of the value to access
 * @return DataType
 */
DataType DatagramHttpServer::getData(const string & key) const try
{
    if (key == "requestId")
    {
        // key is in map!
        return m_reqId;
    }
    else // use base class implementation
    {
        return DatagramMap::getData(key);
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramHttpServer::getData() ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to write data of Datagram under just one name
 *
 * @param key reference name of the value to override
 * @param value value to bet set for member accssible by @p key
 */
void DatagramHttpServer::setData(const string & key, const DataType & value) try
{
    if (key == "requestId")
    {
        setReqId(get<string>(value));
    }
    else // add the key to map
    {
        DatagramMap::setData(key, value);
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramHttpServer::setData(): ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief creates a string representation of this Datagram
 * 
 * @return string 
 */
string DatagramHttpServer::toString() const try
{
    string stringRepresentation = Datagram::toString();
    stringRepresentation += " ReqId=" + m_reqId + " ";
    stringRepresentation += DatagramMap::toString();
    return stringRepresentation;
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramHttpServer::toString(): ") + e.what());
}
