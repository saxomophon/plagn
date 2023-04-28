/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramModbus.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the DatagramModbus class
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

#ifndef DATAGRAMMODBUS_HPP
#define DATAGRAMMODBUS_HPP

// std includes

// own includes
#include "Datagram.hpp"


/**
 * -------------------------------------------------------------------------------------------------
 * @brief short hand as used by the MQTT protocol (see defintion)
 *
 */
enum ModbusDataType: uint8_t
{
    BOOL = 1, //!< Client request to connect to Server
    INT16 = 2, //!< Connect acknowledgment
    UINT16 = 3, //!< Publish message
    INT32 = 4, //!< Publish acknowledgment
    UINT32 = 5, //!< Publish received (assured delivery part 1)
    FLOAT16 = 6, //!< Publish release (assured delivery part 2)
    FLOAT32 = 7, //!< Publish complete (assured delivery part 3)
    FLOAT64 = 8
};

class DatagramModbus: public Datagram
{
public:
    DatagramModbus(const std::string & sourcePlag);
    DatagramModbus(const std::string & sourcePlag, ModbusDataType type,
                   DataType value, uint16_t registerRead);

    ModbusDataType getDataType() const;
    DataType getValue() const;
    uint16_t getRegister() const;

    virtual DataType getData(const std::string & key) const;

    virtual void setData(const std::string & key, const DataType & value);

    virtual std::string toString() const;


private:
    ModbusDataType m_type;  //!< type of the modbus data (within modbus spec)
    DataType m_value;       //!< the data read
    uint16_t m_register;    //!< register the value is read from/written to
};

#endif // DATAGRAMMODBUS_HPP