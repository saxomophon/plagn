/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramMap.hpp
 * @author Fabian Köslin (fabian@koeslin.info)
 * @contributors:
 * @brief Holds the DatagramMap class
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

#ifndef DATAGRAMMAP_HPP
#define DATAGRAMMAP_HPP

// std includes

// own includes
#include "Datagram.hpp"

class DatagramMap : public Datagram
{
public:
    DatagramMap(const std::string & sourcePlag);
    DatagramMap(const std::string & sourcePlag, std::map<std::string, DataType> map);

    virtual DataType getData(const std::string & key) const;

    virtual void setData(const std::string & key, const DataType & value);

    virtual std::string toString() const;

private:
    std::map<std::string, DataType> m_map; //!< map, holding the data of the datagram
};

#endif // DATAGRAMMAP_HPP