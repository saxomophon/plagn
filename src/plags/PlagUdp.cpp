/**
 *-------------------------------------------------------------------------------------------------
 * @file PlagUdp.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements the PlagUdp class
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

// std include
#include <iostream>

// self include
#include "PlagUdp.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Plag Udp:: Plag Udp object assigns default values
 * 
 */
PlagUdp::PlagUdp(const std::string & name, const uint64_t & id) :
    Plag(name, id, PlagType::UDP),
    m_socket(m_ioContext),
    m_resolver(m_ioContext)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Destroy the Plag Udp:: Plag Udp object closes socket, in addition to regular destruct
 * 
 */
PlagUdp::~PlagUdp()
{
    if (!m_stopToken) stopWork();
    try
    {
        m_socket.close();
    }
    catch (exception & e)
    {
        cerr << "Could not close PlagUdp, because of " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Could not close PlagUdp, because for unknown reason!" << endl;
    }
}

void PlagUdp::readConfig() try
{
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagUdo::readConfig()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagUdp::init() configures the UDP socket
 * 
 */
void PlagUdp::init() try
{

    m_socket.open(boost::asio::ip::udp::v4());

    // enable broadcast messages:
    m_socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    m_socket.set_option(boost::asio::socket_base::broadcast(true));

    m_endPoint.address(boost::asio::ip::address_v4::from_string("0.0.0.0"));
    m_endPoint.port(4004);
    
    m_socket.bind(m_endPoint);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagUdo::init()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagUdp::loopWork regularly reads on the socket, if data popped in or sends data
 * 
 */
bool PlagUdp::loopWork() try
{
    // receive what can be received
    if (m_socket.available() > 0)
    {
        char recvBuff[1024] = { 0 };
        boost::asio::ip::udp::endpoint senderEndpoint;
        
        // receive available data
        size_t length = m_socket.receive_from(boost::asio::buffer(recvBuff, 1024), senderEndpoint);
                
        if (length > 0)
        {
            string data = string(recvBuff, length);
            
            cout << "Got data: " << data << endl;
        }
        return true;
    }
    else
    {
        return false;
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened during PlagUdp::loopWork()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief placeDatagram is a function to place a Datagram here. Hence this method decodes the
 * datagram
 *
 * @param datagram A Datagram containing data for this Plag to interprete
 */
void PlagUdp::placeDatagram(const shared_ptr<Datagram> datagram) try
{
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagUdo::placeDatagram()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}