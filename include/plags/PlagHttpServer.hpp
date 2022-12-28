/**
 *-------------------------------------------------------------------------------------------------
 * @file PlagHTTPServer.hpp
 * @author Fabian Koeslin (fabian@koeslin.info)
 * @contributors:
 * @brief Holds the PlagHttpServer, PlagHttpServerTcpServer, PlagHttpServerConnection and Req/Res class
 * @version 0.1
 * @date 2022-12-03
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

#ifndef PLAGHTTPSERVER_HPP
#define PLAGHTTPSERVER_HPP

// std includes
#include <mutex>
#include <string>
#include <list>
#include <map>

// lua includes
#include <lua.hpp>

// boost includes
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>

// own includes
#include "Plag.hpp"
#include "DatagramHttpServer.hpp"
#include "AsyncHttpServer.hpp"
#include "LuaWrapper.hpp"

// forward declaration
class PlagHttpServer;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The PlagHttpServerConnection class Handles one connection to a client async
 *
 */
class PlagHttpServerConnection : public AsyncHttpConnectionInterface
{
public:
    PlagHttpServerConnection(boost::asio::io_context & ioContext, Plag * ptrParentPlag);

    HttpResponse workingRequest(HttpRequest req);
    
private:
    int sendDatagramInterface(lua_State * L); //!< function for sending a dgram from lua
    int resvDatagramInterface(lua_State * L); //!< function for resv a dgram to lua 

    std::vector<std::string> m_reqIds; //!< list of reqIds used in this connection
};


/**
 *-------------------------------------------------------------------------------------------------
 * @brief The PlagUdp class is a Plag to both receive and send data via UDP
 * 
 */
class PlagHttpServer : public Plag
{
public:    
    typedef struct
    {
        AsyncHttpServerUtils::endpoint_t endpointDef;
        std::string workingDirectory;
        std::string scriptFile;
        std::shared_ptr<DatagramHttpServer> stateDgram;

    } endpoint;

    PlagHttpServer(const boost::property_tree::ptree & propTree,
            const std::string & name, const uint64_t & id);
    ~PlagHttpServer();

    virtual void readConfig();

    virtual void init();

    virtual bool loopWork();

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram);

protected:
    // interface for the Lua scripts to use datagrams
    int sendDatagram(std::shared_ptr<DatagramHttpServer> dgram);
    std::shared_ptr<DatagramHttpServer> resvDatagram(const std::vector<std::string> & reqIds);

private:
    // config parameters
    uint16_t m_port;    //!< port the endpoint should bind to
    std::list<endpoint> m_endpoints; //!< list of the configured endpoints
    boost::asio::io_context m_ioContext; //!< io_context for the server
    std::shared_ptr<AsyncHttpServer<PlagHttpServerConnection>> m_tcpServer; //!< pointer that holds the TCP Server
    std::shared_ptr<std::thread> m_ioContextThread; //!< thread for running the io context
    std::mutex m_mtxSending; //!< mutex lock for the sending function
    std::mutex m_mtxRecv; //!< mutex lock for resv function 

    // we need the class PlagHttpServerConnection have access to private 
    // members of this class. So make it friend
    friend class PlagHttpServerConnection;
};
#endif /*PLAGHTTPSERVER_HPP*/