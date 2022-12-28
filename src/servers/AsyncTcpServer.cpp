/**
 *-------------------------------------------------------------------------------------------------
 * @file AsyncTcpServer.cpp
 * @author Fabian Köslin
 * @contributors:
 * @brief Implements the AsyncTcpServer class
 * @version 0.1
 * @date 2022-11-26
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

// std includes

// self include
#include "AsyncTcpServer.hpp"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;


/**
*-------------------------------------------------------------------------------------------------
* @brief Construct a new  AsyncTcpConnectionInterfaces object sets up member values
*
* @param ioContext the boost::io_context
* @param ptrParentPlag ptr to the parent plag
*/
AsyncTcpConnectionInterface::AsyncTcpConnectionInterface(boost::asio::io_context & ioContext,
    Plag * ptrParentPlag)
    : m_sock(ioContext),
    m_ptrParentPlag(ptrParentPlag)
{
}

/**
*-------------------------------------------------------------------------------------------------
* @brief return the boost::socket of the obj
*
*/
tcp::socket & AsyncTcpConnectionInterface::socket()
{
    return m_sock;
}

/**
*-------------------------------------------------------------------------------------------------
* @brief writes some bytes to the socket
*
* @param payload payload to be sent
* @returns number of bytes transfered
*/
size_t AsyncTcpConnectionInterface::writeSome(std::string payload)
{
    return m_sock.write_some(boost::asio::buffer(payload));
}

/**
*-------------------------------------------------------------------------------------------------
* @brief reads some bytes from the socket
*
* @param chucks size if the read buffer
* @returns read bytes 
*/
string AsyncTcpConnectionInterface::readSome(int chunks)
{
    char * buf = new char[chunks];
    size_t len = m_sock.read_some(boost::asio::buffer(buf, chunks));
    string retVal = string(buf, len);
    delete[] buf;
    buf = nullptr;
    return retVal;
}

/**
*-------------------------------------------------------------------------------------------------
* @brief close the socket
*
*/
void AsyncTcpConnectionInterface::closeSock()
{
    m_sock.close();
}





