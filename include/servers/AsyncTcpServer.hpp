/**
 *-------------------------------------------------------------------------------------------------
 * @file AsyncTcpServer.hpp
 * @author Fabian Köslin (fabian@koeslin.info)
 * @contributors:
 * @brief Holds the AsyncTcpServer class
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

#ifndef ASYNCTCPSERVER_HPP_
#define ASYNCTCPSERVER_HPP_

// std includes

// boost includes
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

// own includs
#include "Plag.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The AsyncTcpConnectionInterface class Handles one connection to a client async
 *
 */
class AsyncTcpConnectionInterface:
    public boost::enable_shared_from_this<AsyncTcpConnectionInterface>
{
public:
    AsyncTcpConnectionInterface(boost::asio::io_context & ioContext, Plag * ptrParentPlag);

    boost::asio::ip::tcp::socket & socket();

    virtual void start() = 0;

protected:
    size_t writeSome(std::string payload);
    std::string readSome(int chunks = 1024);
    void closeSock();
    
private:
    boost::asio::ip::tcp::socket m_sock; //!< tcp socket for this connection
    Plag * m_ptrParentPlag; //!< ptr to the parent Plag
};

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The AsyncTcpServer class Handles the connections from a client async
 *
 */
template<class T>
class AsyncTcpServer
{
public:
    /**
    *-------------------------------------------------------------------------------------------------
    * @brief Construct a new  AsyncTcpServer object sets up member values
    *
    * @param io_context the boost::io_context
    * @param port port number of the server
    * @param ptrParentPlag ptr to the parent plag
    */
    AsyncTcpServer(boost::asio::io_context & ioContext, uint16_t port, Plag * ptrParentPlag)
        : m_acceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        m_ioContext(ioContext),
        m_ptrParentPlag(ptrParentPlag)
    {
        startAccept();
    }
    
    /**
    *-------------------------------------------------------------------------------------------------
    * @brief AsyncTcpServer::handleAccept handles a new client connection
    * @param connection ptr to the connection
    * @param err error code, if happend
    */
    void handleAccept(std::shared_ptr<T> connection, const boost::system::error_code & err)
    {
        if (!err)
        {
            connection->start();
        }
        startAccept();
    }

protected:
    /**
    *-------------------------------------------------------------------------------------------------
    * @brief AsncTcpServer::startAccept accepts a new client connection
    *
    */
    void startAccept()
    {
        // socket
        auto connection = std::shared_ptr<T>(new T(m_ioContext, m_ptrParentPlag));

        // asynchronous accept operation and wait for a new connection.
        m_acceptor.async_accept(connection->socket(),
            boost::bind(&AsyncTcpServer::handleAccept, this, connection,
                boost::asio::placeholders::error));
    }
    
    boost::asio::ip::tcp::acceptor m_acceptor; //!< acceptor for the incoming connections
    boost::asio::io_context & m_ioContext; //!< io_service for the server
    Plag * m_ptrParentPlag; //!< ptr to the parent plug of this server
};

#endif /*ASYNCTCPSERVER_HPP_*/