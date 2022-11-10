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
    PlagUdp();
    ~PlagUdp();

    virtual void readConfig();

    virtual void init();

    virtual void loopWork();

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram);

protected:

private:
    // to correctly interprete the following members, see the boost documentation
    boost::asio::io_context m_ioContext;            //!< member of boost necessity for ease of use
    boost::asio::ip::udp::socket m_socket;          //!< member of boost necessity for ease of use
    boost::asio::ip::udp::resolver m_resolver;      //!< member of boost necessity for ease of use
    boost::asio::ip::udp::endpoint m_endPoint;      //!< member of boost necessity for ease of use
};

#endif // PLAGUDP_HPP
