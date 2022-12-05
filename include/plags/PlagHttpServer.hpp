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


// boost includes
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>

// own includes
#include "Plag.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The PlagHttpServerConnection class Handles one connection to a client async
 *
 */
class PlagHttpServerConnection : public boost::enable_shared_from_this<PlagHttpServerConnection>
{
public:
    PlagHttpServerConnection(boost::asio::io_context & ioContext);
    
    typedef boost::shared_ptr<PlagHttpServerConnection> pointer;
    
    static pointer create(boost::asio::io_context & ioContext);

    boost::asio::ip::tcp::socket & socket();

    void start();

private:
    boost::asio::ip::tcp::socket m_sock; //!< tcp socket for this connection

    std::string getRawRequest(); //!< function for getting the raw request from sock
};

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The PlagHTTPServerTcpServer class Handles the connections from a client async
 *
 */
class PlagHttpServerTcpServer
{
public:
    PlagHttpServerTcpServer(boost::asio::io_context & s, uint16_t port);
    void handleAccept(PlagHttpServerConnection::pointer connection, const boost::system::error_code & err);

private:
    void startAccept();
    boost::asio::ip::tcp::acceptor m_acceptor; //!< acceptor for the incoming connections
    boost::asio::io_context & m_ioContext; //!< io_service for the server
};

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The PlagUdp class is a Plag to both receive and send data via UDP
 * 
 */
class PlagHttpServer : public Plag
{
public:
    // some typedefs
    typedef enum
    {
        HTTP_UNKNOWN = 0,
        HTTP_GET = 1,
        HTTP_POST = 2,
        HTTP_PUT = 3,
        HTTP_DELETE = 4,
        HTTP_HEAD = 5,
        HTTP_CONNECT = 6,
        HTTP_OPTIONS = 7,
        HTTP_TRACE = 8,
        HTTP_PATCH = 9
    } httpMethod;
    
    typedef struct
    {
        std::string endpoint;
        std::string scriptFile;
        httpMethod method;

    } endpoint;

    // http response code, type and statics
    typedef struct
    {
        int code;
        std::string message;
    } responseStatusCode;

    static const responseStatusCode HTTP_100; //!< 100 Continue
    static const responseStatusCode HTTP_101; //!< 101 Switching Protocols
    static const responseStatusCode HTTP_103; //!< 103 Early Hints
    static const responseStatusCode HTTP_200; //!< 200 OK
    static const responseStatusCode HTTP_201; //!< 201 Created
    static const responseStatusCode HTTP_202; //!< 202 Accepted
    static const responseStatusCode HTTP_203; //!< 203 Non - Authoritative Information
    static const responseStatusCode HTTP_204; //!< 204 No Content
    static const responseStatusCode HTTP_205; //!< 205 Reset Content
    static const responseStatusCode HTTP_206; //!< 206 Partial Content
    static const responseStatusCode HTTP_300; //!< 300 Multiple Choices
    static const responseStatusCode HTTP_301; //!< 301 Moved Permanently
    static const responseStatusCode HTTP_302; //!< 302 Found
    static const responseStatusCode HTTP_303; //!< 303 See Other
    static const responseStatusCode HTTP_304; //!< 304 Not Modified
    static const responseStatusCode HTTP_307; //!< 307 Temporary Redirect
    static const responseStatusCode HTTP_308; //!< 308 Permanent Redirect
    static const responseStatusCode HTTP_400; //!< 400 Bad Request
    static const responseStatusCode HTTP_401; //!< 401 Unauthorized
    static const responseStatusCode HTTP_402; //!< 402 Payment Required
    static const responseStatusCode HTTP_403; //!< 403 Forbidden
    static const responseStatusCode HTTP_404; //!< 404 Not Found
    static const responseStatusCode HTTP_405; //!< 405 Method Not Allowed
    static const responseStatusCode HTTP_406; //!< 406 Not Acceptable
    static const responseStatusCode HTTP_407; //!< 407 Proxy Authentication Required
    static const responseStatusCode HTTP_408; //!< 408 Request Timeout
    static const responseStatusCode HTTP_409; //!< 409 Conflict
    static const responseStatusCode HTTP_410; //!< 410 Gone
    static const responseStatusCode HTTP_411; //!< 411 Length Required
    static const responseStatusCode HTTP_412; //!< 412 Precondition Failed
    static const responseStatusCode HTTP_413; //!< 413 Payload Too Large
    static const responseStatusCode HTTP_414; //!< 414 URI Too Long
    static const responseStatusCode HTTP_415; //!< 415 Unsupported Media Type
    static const responseStatusCode HTTP_416; //!< 416 Range Not Satisfiable
    static const responseStatusCode HTTP_417; //!< 417 Expectation Failed
    static const responseStatusCode HTTP_418; //!< 418 I'm a teapot
    static const responseStatusCode HTTP_425; //!< 425 Too Early
    static const responseStatusCode HTTP_426; //!< 426 Upgrade Required
    static const responseStatusCode HTTP_428; //!< 428 Precondition Required
    static const responseStatusCode HTTP_429; //!< 429 Too Many Requests
    static const responseStatusCode HTTP_431; //!< 431 Request Header Fields Too Large
    static const responseStatusCode HTTP_451; //!< 451 Unavailable For Legal Reasons
    static const responseStatusCode HTTP_500; //!< 500 Internal Server Error
    static const responseStatusCode HTTP_501; //!< 501 Not Implemented
    static const responseStatusCode HTTP_502; //!< 502 Bad Gateway
    static const responseStatusCode HTTP_503; //!< 503 Service Unavailable
    static const responseStatusCode HTTP_504; //!< 504 Gateway Timeoutå
    static const responseStatusCode HTTP_505; //!< 505 HTTP Version Not Supported
    static const responseStatusCode HTTP_506; //!< 506 Variant Also Negotiates
    static const responseStatusCode HTTP_510; //!< 510 Not Extended
    static const responseStatusCode HTTP_511; //!< 511 Network Authentication Required

    PlagHttpServer(const boost::property_tree::ptree & propTree,
            const std::string & name, const uint64_t & id);
    ~PlagHttpServer();

    virtual void readConfig();

    virtual void init();

    virtual bool loopWork();

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram);

protected:
    // interface for the Lua scripts to use datagrams
    void sendDatagram(void); // TODO: the new defined datagram has to be put here
    void getDatagram(void); // TODO: the new defined datagram has to be put here


private:
    // config parameters
    uint16_t m_port;    //!< port the endpoint should bind to
    std::list<endpoint> m_endpoints; //!< list of the configured endpoints
    boost::asio::io_context m_ioContext; //!< io_context for the server
    boost::shared_ptr<PlagHttpServerTcpServer> m_tcpServer; //!< pointer that holds the TCP Server

    // we need the class PlagHttpServerConnection have access to private 
    // members of this class. So make it friend
    friend class PlagHttpServerConnection;
};

class PlagHttpServerHttpData
{
public:
    boost::optional<std::string> getHeader(std::string key);
    std::map<std::string, std::string> getHeader();
    std::string getContent();
    std::string getHttpVersion();
    std::string getEndpoint();
    PlagHttpServer::httpMethod getMethod();
    
protected:
    PlagHttpServer::httpMethod m_method;
    std::string m_version;
    std::map<std::string, std::string> m_header;
    std::string m_content;
    std::string m_endpoint;
};

class PlagHttpServerHttpRequest : public PlagHttpServerHttpData
{
public:
    PlagHttpServerHttpRequest(std::string rawRequest);
    boost::optional<std::string> getParam(std::string key);
    std::map<std::string, std::string> getParams();
private:
    std::map<std::string, std::string> m_params;
};

class PlagHttpServerHttpResponse : public PlagHttpServerHttpData
{
public:
    PlagHttpServerHttpResponse(PlagHttpServer::httpMethod method, std::string version, std::string endpoint);
    void addHeader(std::string key, std::string value);
    void setContent(std::string content);
    void setStatus(PlagHttpServer::responseStatusCode status);
    std::string encode();

private:
    PlagHttpServer::responseStatusCode m_status;
};


#endif // PLAGHTTPSERVER_HPP
