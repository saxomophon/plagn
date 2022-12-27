/**
 *-------------------------------------------------------------------------------------------------
 * @file TcpClient.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements the TcpClient class
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
#include <system_error>

// self include
#include "TcpClient.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Tcp Client:: Tcp Client object sets up member values
 * 
 * @param timeout time until any operation is deemed to have failed (e.g. TcpClient::receive())
 * @param parent the parent Plag this TransportLayer interface reports to
 * @param serverIP the IP of the server this client connects to
 * @param port port number under whicht to connect to server
 */
TcpClient::TcpClient(const std::chrono::milliseconds & timeout,
                     const Plag & parent, const string & serverIP, uint16_t port) try :
    TransportLayer(timeout),
    m_parent(parent),
    m_endpoint(boost::asio::ip::address::from_string(serverIP), port),
    m_ioContext(),
    m_socket(nullptr),
    m_receiveBuffer(""),
    m_isConnected(false)
{
    m_type = TCP_CLIENT;
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::TcpClient : ") + e.what());
}

/**
 *--------------------------------------------------------------------------------------------------
 * @brief attempt a TCP connection using the given member data, set up at construction
 * 
 * @param timeout user may set timeout, if this takes longer than the default-value (1000 => 1s).
 */
void TcpClient::connect(const std::chrono::milliseconds & timeout) try
{
    if (!isConnected())
    {
        m_ioContext.reset();
        m_socket.reset(new boost::asio::ip::tcp::socket(m_ioContext));
        m_socket->async_connect(m_endpoint,
                                boost::bind(&TcpClient::handleBoostConnect, this,
                                            boost::placeholders::_1));

        m_ioContext.run_one_for(std::chrono::milliseconds(timeout.count()));
        m_ioContext.poll();
        if (!isConnected())
        {
            m_socket->close();
            m_ioContext.run_for(std::chrono::milliseconds(timeout.count()));
        }
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::connect : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief closes socket, empties async interface and resets socket
 * 
 */
void TcpClient::disconnect() try
{
    if (m_socket != nullptr)
    {
        try { m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both); }
        catch (...) {}

        try { m_socket->close(); }
        catch (...) {}

        try { m_ioContext.poll(); }
        catch (...) {}

        try { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
        catch (...) {}

        try { m_ioContext.poll(); }
        catch (...) {}

        m_socket.reset();
    }
    m_isConnected = false;
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::disconnect : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Checks current connection status by polling socket
 * 
 * @return true if no disconnect was triggered (i.e. socket is still open)
 * @return false if disconnect() was triggered
 * @sa TcpClient::disconnect()
 */
bool TcpClient::isConnected() try
{
    m_ioContext.poll();
    if (m_socket == nullptr || !m_socket->is_open()) disconnect();
    return m_isConnected;
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::isConnected : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Check for number of bytes received on socket and return that.
 * 
 * @return size_t Number of bytes, received on socket and writte from asyn buffer to this' buffer
 */
size_t TcpClient::getAvailableBytesCount() try
{
    m_ioContext.poll();
    return m_receiveBuffer.size();
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::getAvailableBytesCount : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Waits for @p numberOfBytes on socket and returns them within timeout-limits.
 * Otherwise throws.
 *
 * @param numberOfBytes number of bytes to wait for and return
 * @return string returns @p numberOfBytes , or all available bytes, if @p numberOfBytes == 0
 */
string TcpClient::receiveBytes(size_t numberOfBytes) try
{
    if (numberOfBytes == 0) numberOfBytes = this->getAvailableBytesCount();

    std::chrono::time_point<std::chrono::steady_clock> timeoutP = std::chrono::steady_clock::now()
                                                                  + m_timeout;

    while (this->getAvailableBytesCount() < numberOfBytes
           && (m_timeout == std::chrono::milliseconds(0)
               || timeoutP > std::chrono::steady_clock::now()))
    {
        if (!isConnected()) throw std::runtime_error("Cannot receive when not connected!");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (this->getAvailableBytesCount() < numberOfBytes) throw std::runtime_error("Reached Timeout!");

    string requestedData = m_receiveBuffer.substr(0, numberOfBytes);
    m_receiveBuffer.erase(0, numberOfBytes);

    return requestedData;
}
catch (std::runtime_error & e)
{
    disconnect();
    throw std::runtime_error(string("Happened in TcpClient::receiveBytes : ") + e.what());
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::receiveBytes : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Peeks, if enough data is available and only then returns data, otherwise "".
 * @details Use this function in a while (peekAndReceive( @p numberOfBytes ) == "")-loop, to wait
 * for a certain number of bytes
 *
 * @param numberOfBytes the number of bytes wanted to receive
 * @return string Hence returns exactly @p numberOfBytes from the socket.
 * If not enough data is available, returns ""
 */
string TcpClient::peekAndReceive(size_t numberOfBytes) try
{
    if (getAvailableBytesCount() < numberOfBytes || numberOfBytes == 0) return "";
    return receiveBytes(numberOfBytes);
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::peekAndReceive : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief sends your @p appData to the server this client is connected to
 * 
 * @param appData short for application data. The data you want to send via TCP/IP
 */
void TcpClient::transmit(const string & appData) try
{
    if (!isConnected()) throw std::runtime_error("Cannot transmit, when not connected!");
    m_socket->write_some(boost::asio::buffer(appData));
}
catch (std::runtime_error & e)
{
    disconnect();
    throw std::runtime_error(string("Happened in TcpClient::transmit : ") + e.what());
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::transmit : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Gets called, when boost's connect operation returns and acts accordingly
 * @details If connect operation is successful, sets connected state to true and inits
 * receive operation. Otherwise re-throws the boost error
 *
 * @param error a boost error code, remarking either a successful return or a failure (e.g. connection rejected)
 * @sa TcpClient::intiBoostReceive()
 */
void TcpClient::handleBoostConnect(const boost::system::error_code & error) try
{
    if (error)
    {
        // async_connect failed
        throw boost::system::system_error(error);
    }
    else
    {
        // async_connect was successful
        m_isConnected = true;
        initBoostReceive();
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::handleBoostConnect : ") + e.what());
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief initiates an async_receive operation on the socket, if there is a connection
 */
void TcpClient::initBoostReceive() try
{
    if (!isConnected()) throw std::runtime_error("Cannot start receiver, when not connected");
    m_socket->async_receive(boost::asio::buffer(m_boostsReceiveBuffer, RECEIVE_BUFFER_SIZE),
                            boost::bind(&TcpClient::handleBoostReceive, this,
                                        boost::placeholders::_1, boost::placeholders::_2));
}
catch (std::runtime_error & e)
{
    disconnect();
    throw std::runtime_error(string("Happened in TcpClient::initBoostReceive : ") + e.what());
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::initBoostReceive : ") + e.what());
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief TcpClient::handleBoostReceive is called when async_receive yields data (or fails)
 *
 * @param error a boost error code, remarking either a successful return or a failure (e.g. connection closed)
 * @param n number of bytes received
 */
void TcpClient::handleBoostReceive(const boost::system::error_code & error, std::size_t n) try
{
    if (error)
    {
        throw boost::system::system_error(error);
    }
    else
    {
        m_receiveBuffer.append(m_boostsReceiveBuffer, n); // reads n bytes from async to internal buffer
        initBoostReceive();                              // start next receive "interval"
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TcpClient::handleBoostReceive : ") + e.what());
}
