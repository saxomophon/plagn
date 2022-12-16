/**
 *-------------------------------------------------------------------------------------------------
 * @file TlsClient.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the TlsClient class
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

#ifndef TLSCLIENT_HPP_
#define TLSCLIENT_HPP_

// std includes

// boost includes
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

// own includs
#include "Plag.hpp"
#include "TransportLayer.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Imlements a TCP client as an implementation of the TransportLayer interface using boost
 * @sa TransportLayer::TransportLayer
 */
class TlsClient : public TransportLayer
{
public:
    TlsClient(const std::chrono::milliseconds & timeout, const Plag & parent,
              const std::string & serverIP, uint16_t port, const std::string & certFile,
              bool verify = false);

    virtual void connect(const std::chrono::milliseconds & timeout = std::chrono::milliseconds(1000));
    virtual void disconnect();
    virtual bool isConnected();

    virtual size_t getAvailableBytesCount();
    
    virtual std::string receiveBytes(size_t numberOfBytes = 0);
    virtual std::string peekAndReceive(size_t numberOfBytes);

    virtual void transmit(const std::string & appData);

private:
    // methods:
    void handleBoostConnect(const boost::system::error_code & error);
    void performHandshake();
    void handleSslHandshake(const boost::system::error_code & error);
    void initBoostReceive();
    void handleBoostReceive(const boost::system::error_code & error, std::size_t n);

private:
    const Plag & m_parent;                                  //!< the parent Plag, holding this layer
    std::string m_certFile;                                 //!< file to private certificate
    boost::asio::ip::tcp::endpoint m_endpoint;              //!< TCP/IP endpoint to connect to (IP and port)
    boost::asio::io_context m_ioContext;                    //!< boost interface object for async operations
    boost::asio::ssl::context m_sslContext;                 //!< boost interface object for async operations
    std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket &>> m_socket; //!< socket, representing an open connection
    std::string m_receiveBuffer;                            //!< buffer for this as interface to Plag
    bool m_isConnected;                                     //!< state: is this connected to server
    bool m_handIsShaken;                                    //!< state: is the handshake succesfully performed
    static const size_t RECEIVE_BUFFER_SIZE = 4096;         //!< buffer size for async receiv operations
    char m_boostsReceiveBuffer[RECEIVE_BUFFER_SIZE];        //!< buffer for boost's async receive operations
};

#endif /*TCPCLIENT_HPP_*/