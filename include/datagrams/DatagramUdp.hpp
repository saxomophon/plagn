/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramUdp.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the DatagramUdp class
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

#ifndef DATAGRAMUDP_HPP
#define DATAGRAMUDP_HPP

// std includes

// own includes
#include "Datagram.hpp"

class DatagramUdp : public Datagram
{
public:
    DatagramUdp(const std::string & sourcePlag);
    DatagramUdp(const std::string & sourcePlag, const std::string & sender,
                unsigned int port, const std::string & payload);

    const std::string & getSender() const;
    const std::string & getReceiver() const;
    unsigned int getPort() const;
    const std::string & getPayload() const;

    virtual DataType getData(const std::string & key) const;

    virtual void setData(const std::string & key, const DataType & value);

    virtual std::string toString() const;


private:
    std::string m_sender;   //!< identification of sender (most likely IP) as a string
    std::string m_receiver; //!< identification of receiver (most likely IP) as a string
    uint16_t m_port;        //!< port of the UDP interaction
    std::string m_payload;  //!< content of the UDP
};

#endif // DATAGRAMUDP_HPP