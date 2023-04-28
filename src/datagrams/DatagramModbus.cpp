/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramModbus.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implementation of the DatagramModbus class
 * @version 0.1
 * @date 2023-04-26
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
#include "DatagramModbus.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Datagram:: Datagram object sets member values
 *
 * @param sourcePlag the origin of this Datagram
 */
DatagramModbus::DatagramModbus(const string & sourcePlag):
    Datagram(sourcePlag),
    m_type(ModbusDataType::BOOL),
    m_value(int(0)),
    m_register(0)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief overload to Construct a new Datagram:: Datagram object sets member values
 *
 * @param sourcePlag the origin of this Datagram
 */
DatagramModbus::DatagramModbus(const string & sourcePlag, ModbusDataType type,
                               DataType value, uint16_t registerRead):
    Datagram(sourcePlag),
    m_type(type),
    m_value(value),
    m_register(registerRead)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return ModbusDataType copy of member value
 */
ModbusDataType DatagramModbus::getDataType() const
{
    return m_type;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return DataType copy of member value
 */
DataType DatagramModbus::getValue() const
{
    return m_value;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 *
 * @return uint16_t copy of member value
 */
uint16_t DatagramModbus::getRegister() const
{
    return m_register;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to access data of Datagram under just one name
 *
 * @param key reference name of the value to access
 * @return DataType
 */
DataType DatagramModbus::getData(const string & key) const try
{
    if (key == string("type"))
    {
        return getDataType();
    }
    else if (key == string("value"))
    {
        return getValue();
    }
    else if (key == string("register"))
    {
        return getRegister();
    }
    else // use base class implementation
    {
        return Datagram::getData(key);
    }
}
catch (std::invalid_argument & e)
{
    throw std::invalid_argument(string("In DatagramModbus: ") + e.what());
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramModbus::getData() ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to write data of Datagram under just one name
 *
 * @param key reference name of the value to override
 * @param value value to bet set for member accssible by @p key
 */
void DatagramModbus::setData(const string & key, const DataType & value) try
{
    if (key == string("type"))
    {
        m_type = static_cast<ModbusDataType>(static_cast<uint8_t>(convertDataTypeToUint(value)));
    }
    else if (key == string("value"))
    {
        m_value = value;
    }
    else if (key == string("register"))
    {
        m_register = static_cast<uint16_t>(convertDataTypeToUint(value));
    }
    else // use base class implementation
    {
        Datagram::setData(key, value);
    }
}
catch (std::invalid_argument & e)
{
    throw std::invalid_argument(string("In DatagramModbus: ") + e.what());
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramModbus::setData(): ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief creates a string representation of this Datagram
 *
 * @return string
 */
string DatagramModbus::toString() const try
{
    string stringRepresentation = Datagram::toString();
    stringRepresentation += "{Modbus info: type: " + to_string(m_type);
    stringRepresentation += "; value: " + convertDataTypeToString(m_value);
    stringRepresentation += "; reigser: " + to_string(m_register);
    stringRepresentation += "}";
    return stringRepresentation;
}
catch (exception & e)
{
    throw std::runtime_error(string("DatagramModbus::toString(): ") + e.what());
}
