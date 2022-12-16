/**
 *-------------------------------------------------------------------------------------------------
 * @file TlsClient.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements the TlsClient class
 * @version 0.1
 * @date 2022-12-15
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
#include "TlsClient.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Tcp Client:: Tcp Client object sets up member values
 * 
 * @param timeout time until any operation is deemed to have failed (e.g. TlsClient::receive())
 * @param parent the parent Plag this TransportLayer interface reports to
 * @param serverIP the IP of the server this client connects to
 * @param port port number under whicht to connect to server
 */
TlsClient::TlsClient(const std::chrono::milliseconds & timeout, const Plag & parent,
                     const string & serverIP, uint16_t port, const string & certFile,
                     bool verify) try:
    TransportLayer(timeout),
    m_parent(parent),
    m_certFile(certFile),
    m_endpoint(boost::asio::ip::address::from_string(serverIP), port),
    m_ioContext(),
    m_sslContext(boost::asio::ssl::context::sslv23),
    m_socket(nullptr),
    m_receiveBuffer(""),
    m_isConnected(false)
{
    m_type = TLS_CLIENT;
    if (verify)
    {
        m_sslContext.set_verify_mode(boost::asio::ssl::context::verify_peer);
    }
    else
    {
        m_sslContext.set_verify_mode(boost::asio::ssl::context::verify_none);
    }
    if (m_certFile == "")
    {
        m_sslContext.set_default_verify_paths();
    }
    else
    {
        m_sslContext.load_verify_file(m_certFile);
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::TlsClient : ") + e.what());
}

/**
 *--------------------------------------------------------------------------------------------------
 * @brief attempt a TCP connection using the given member data, set up at construction
 * 
 * @param timeout user may set timeout, if this takes longer than the default-value (1000 => 1s).
 */
void TlsClient::connect(const std::chrono::milliseconds & timeout) try
{
    if (!isConnected())
    {
        m_ioContext.reset();

        boost::asio::ip::tcp::socket tcpSocket(m_ioContext);
        m_socket.reset(new boost::asio::ssl::stream<boost::asio::ip::tcp::socket &>(tcpSocket,
                                                                                    m_sslContext));
        m_socket->lowest_layer().async_connect(m_endpoint,
                                               boost::bind(&TlsClient::handleBoostConnect, this,
                                                           boost::placeholders::_1));

        m_ioContext.run_one_for(std::chrono::milliseconds(timeout.count()));
        m_ioContext.poll();
        if (!isConnected())
        {
            m_socket->lowest_layer().close();
            m_ioContext.run_for(std::chrono::milliseconds(timeout.count()));
        }
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::connect : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief closes socket, empties async interface and resets socket
 * 
 */
void TlsClient::disconnect() try
{
    if (m_socket != nullptr)
    {
        try { m_socket->shutdown(); }
        catch (...) {}

        try { m_socket->lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both); }
        catch (...) {}

        try { m_socket->lowest_layer().close(); }
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
    throw std::runtime_error(string("Happened in TlsClient::disconnect : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Checks current connection status by polling socket
 * 
 * @return true if no disconnect was triggered (i.e. socket is still open)
 * @return false if disconnect() was triggered
 * @sa TlsClient::disconnect()
 */
bool TlsClient::isConnected() try
{
    m_ioContext.poll();
    if (m_socket == nullptr || !m_socket->lowest_layer().is_open()) disconnect();
    return m_isConnected && m_handIsShaken;
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::isConnected : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Check for number of bytes received on socket and return that.
 * 
 * @return size_t Number of bytes, received on socket and writte from asyn buffer to this' buffer
 */
size_t TlsClient::getAvailableBytesCount() try
{
    m_ioContext.poll();
    return m_receiveBuffer.size();
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::getAvailableBytesCount : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Waits for @p numberOfBytes on socket and returns them within timeout-limits.
 * Otherwise throws.
 *
 * @param numberOfBytes number of bytes to wait for and return
 * @return string returns @p numberOfBytes , or all available bytes, if @p numberOfBytes == 0
 */
string TlsClient::receiveBytes(size_t numberOfBytes) try
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
    throw std::runtime_error(string("Happened in TlsClient::receiveBytes : ") + e.what());
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::receiveBytes : ") + e.what());
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
string TlsClient::peekAndReceive(size_t numberOfBytes) try
{
    if (getAvailableBytesCount() < numberOfBytes || numberOfBytes == 0) return "";
    return receiveBytes(numberOfBytes);
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::peekAndReceive : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief sends your @p appData to the server this client is connected to
 * 
 * @param appData short for application data. The data you want to send via TCP/IP
 */
void TlsClient::transmit(const string & appData) try
{
    if (!isConnected()) throw std::runtime_error("Cannot transmit, when not connected!");
    m_socket->write_some(boost::asio::buffer(appData));
}
catch (std::runtime_error & e)
{
    disconnect();
    throw std::runtime_error(string("Happened in TlsClient::transmit : ") + e.what());
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::transmit : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Gets called, when boost's connect operation returns and acts accordingly
 * @details If connect operation is successful, sets connected state to true and inits
 * receive operation. Otherwise re-throws the boost error
 *
 * @param error a boost error code, remarking either a successful return or a failure (e.g. connection rejected)
 * @sa TlsClient::intiBoostReceive()
 */
void TlsClient::handleBoostConnect(const boost::system::error_code & error) try
{
    if (error)
    {
        // async_connect failed
        throw boost::system::system_error(error);
    }
    // async_connect was successful
    m_isConnected = true;
    performHandshake();
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::handleBoostConnect : ") + e.what());
}

void TlsClient::performHandshake() try
{
    if (!m_isConnected) throw std::runtime_error("Cannot perform handshake, when not connected");
    m_socket->async_handshake(boost::asio::ssl::stream_base::client,
                              boost::bind(&TlsClient::handleSslHandshake, this,
                                          boost::placeholders::_1));
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::performHandshake : ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Gets called, when boost's connect operation returns and acts accordingly
 * @details If connect operation is successful, sets connected state to true and inits
 * receive operation. Otherwise re-throws the boost error
 *
 * @param error a boost error code, remarking either a successful return or a failure (e.g. connection rejected)
 * @sa TlsClient::intiBoostReceive()
 */
void TlsClient::handleSslHandshake(const boost::system::error_code & error) try
{
    if (error)
    {
        // async_connect failed
        throw boost::system::system_error(error);
    }
    // async_connect was successful
    m_handIsShaken = true;
    initBoostReceive();
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::handleBoostConnect : ") + e.what());
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief initiates an async_receive operation on the socket, if there is a connection
 */
void TlsClient::initBoostReceive() try
{
    if (!isConnected()) throw std::runtime_error("Cannot start receiver, when not connected");
    m_socket->async_read_some(boost::asio::buffer(m_boostsReceiveBuffer, RECEIVE_BUFFER_SIZE),
                                                  boost::bind(&TlsClient::handleBoostReceive, this,
                                                              boost::placeholders::_1,
                                                              boost::placeholders::_2));
}
catch (std::runtime_error & e)
{
    disconnect();
    throw std::runtime_error(string("Happened in TlsClient::initBoostReceive : ") + e.what());
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in TlsClient::initBoostReceive : ") + e.what());
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief TlsClient::handleBoostReceive is called when async_receive yields data (or fails)
 *
 * @param error a boost error code, remarking either a successful return or a failure (e.g. connection closed)
 * @param n number of bytes received
 */
void TlsClient::handleBoostReceive(const boost::system::error_code & error, std::size_t n) try
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
    throw std::runtime_error(string("Happened in TlsClient::handleBoostReceive : ") + e.what());
}
