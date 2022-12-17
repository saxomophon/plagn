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

AsyncTcpConnectionInterfaces::AsyncTcpConnectionInterfaces(boost::asio::io_context & ioContext,
    Plag * ptrParentPlag)
    : m_sock(ioContext),
    m_ptrParentPlag(ptrParentPlag)
{
}

tcp::socket & AsyncTcpConnectionInterfaces::socket()
{
    return m_sock;
}

size_t AsyncTcpConnectionInterfaces::writeSome(std::string payload)
{
    return m_sock.write_some(boost::asio::buffer(payload));
}

string AsyncTcpConnectionInterfaces::readSome(int chunks = 1024)
{
    char * buf = new char[chunks];
    size_t len = m_sock.read_some(boost::asio::buffer(buf, chunks));
    string retVal = string(buf, len);
    delete[] buf;
    buf = nullptr;
    return retVal;
}

void AsyncTcpConnectionInterfaces::closeSock()
{
    m_sock.close();
}





