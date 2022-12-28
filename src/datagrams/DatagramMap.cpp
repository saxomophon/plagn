/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramMap.cpp
 * @author Fabian Köslin (fabian@koeslin.info)
 * @contributors: 
 * @brief Implementation of the DatagramMap class
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
#include "DatagramMap.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Datagram:: Datagram object sets member values
 * 
 * @param sourcePlag the origin of this Datagram
 */
DatagramMap::DatagramMap(const string & sourcePlag) :
    Datagram(sourcePlag)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief overload to Construct a new Datagram:: Datagram object sets member values
 *
 * @param sourcePlag the origin of this Datagram
 */
DatagramMap::DatagramMap(const string & sourcePlag, map<string, DataType> map) :
    Datagram(sourcePlag),
    m_map(map)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to access data of Datagram under just one name
 *
 * @param key reference name of the value to access
 * @return DataType
 */
DataType DatagramMap::getData(const string & key) const try
{
    if (m_map.find(key) != m_map.end())
    {
        // key is in map!
        return m_map.at(key);
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
void DatagramMap::setData(const string & key, const DataType & value) try
{
    if (m_map.find(key) != m_map.end())
    {
        // key is in map and can be override
        m_map[key] = value;
    } 
    else // add the key to map
    {
        m_map.insert(pair<string, DataType>(key, value));
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramMap::setData(): ") + e.what());
}

const std::map<std::string, DataType> & DatagramMap::getMap()
{
    return m_map;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief creates a string representation of this Datagram
 * 
 * @return string 
 */
string DatagramMap::toString() const try
{
    string stringRepresentation = Datagram::toString();
    stringRepresentation += "{Map info: ";
    for (auto pair : m_map)
    {
        stringRepresentation += pair.first + "=";
        if (const int * pvar = get_if<int>(&pair.second))
        {
            // value is int!
            stringRepresentation += to_string(*pvar);
        }
        else if (const unsigned int * pvar = get_if<unsigned int>(&pair.second))
        {
            // value is uint!
            stringRepresentation += to_string(*pvar);
        }
        else if (const int64_t * pvar = get_if<int64_t>(&pair.second))
        {
            // value is int64_t!
            stringRepresentation += to_string(*pvar);
        }
        else if (const uint64_t * pvar = get_if<uint64_t>(&pair.second))
        {
            // value is uint64_t!
            stringRepresentation += to_string(*pvar);
        }
        else if (const double * pvar = get_if<double>(&pair.second))
        {
            // value is double!
            stringRepresentation += to_string(*pvar);
        }
        else if (const string * pvar = get_if<string>(&pair.second))
        {
            // value is string!
            stringRepresentation += *pvar;
        }
        else if (const map<string, string> * pvar = get_if<map<string, string> >(&pair.second))
        {
            // value is map<string, string>!
            stringRepresentation += "{";
            for (auto innerPair : *pvar)
            {
                stringRepresentation += innerPair.first + "=" + innerPair.second + ", ";
            }
            stringRepresentation += "}";
        }
        else if (const vector<string> * pvar = get_if<vector<string> >(&pair.second))
        {
            // value is map<string, string>!
            stringRepresentation += "{";
            for (auto element : *pvar)
            {
                stringRepresentation += element + ", ";
            }
            stringRepresentation += "}";
        }
        stringRepresentation +=  ", ";
    }
    stringRepresentation += "}";
    return stringRepresentation;
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramUdp::toString(): ") + e.what());
}
