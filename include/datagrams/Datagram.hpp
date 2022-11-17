/**
 *-------------------------------------------------------------------------------------------------
 * @file Datagram.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the Datagram class
 * @version 0.1
 * @date 2022-20-05
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

#ifndef DATAGRAM_HPP
#define DATAGRAM_HPP

// std includes
#include <chrono>
#include <string>

// own includes

class Datagram
{
public:
    Datagram(const std::string & sourceName);

    virtual std::string toString() const;


private:
    std::string m_ownId;                                                       //!< generated unique DatagramId
    uint64_t m_sourceDatagramId;                                            //!< id of Datagram this was translated from (if new: 0)
    std::chrono::time_point<std::chrono::steady_clock> m_timeOfCreation;    //!< time this Datagram was created

};

// interface to std string representations
std::ostream & operator<<(std::ostream & os, const std::shared_ptr<Datagram> datagram);
std::string to_string(const std::shared_ptr<Datagram> datagram);

#endif // DATAGRAM_HPP