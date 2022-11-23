/**
 *-------------------------------------------------------------------------------------------------
 * @file PlagUdp.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the PlagUdp class
 * @version 0.1
 * @date 2022-11-10
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

#ifndef PLAGUDP_HPP
#define PLAGUDP_HPP

// std includes

// boost includes
#include <boost/asio.hpp>

// own includes
#include "Plag.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The PlagUdp class is a Plag to both receive and send data via UDP
 * 
 */
class PlagUdp : public Plag
{
public:
    PlagUdp(const boost::property_tree::ptree & propTree,
            const std::string & name, const uint64_t & id);
    ~PlagUdp();

    virtual void readConfig();

    virtual void init();

    virtual bool loopWork();

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram);

protected:

private:
    // config parameters
    std::string m_ip;   //!< ip the endpoint should bind to
    uint16_t m_port;    //!< port the endpoint should bind to
    // to correctly interprete the following members, see the boost documentation
    boost::asio::io_context m_ioContext;            //!< member of boost necessity for ease of use
    boost::asio::ip::udp::socket m_socket;          //!< member of boost necessity for ease of use
    boost::asio::ip::udp::resolver m_resolver;      //!< member of boost necessity for ease of use
    boost::asio::ip::udp::endpoint m_endPoint;      //!< member of boost necessity for ease of use
};

#endif // PLAGUDP_HPP
