/**
 *-------------------------------------------------------------------------------------------------
 * @file AsyncHttpServer.hpp
 * @author Fabian Köslin (fabian@koeslin.info)
 * @contributors:
 * @brief Holds the AsyncHttpServer class
 * @version 0.1
 * @date 2022-12-18
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

#ifndef ASYNCHTTPSERVER_HPP_
#define ASYNCHTTPSERVER_HPP_

// std includes

// boost includes
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

// own includs
#include "Plag.hpp"
#include "AsyncTcpServer.hpp"
#include "AsyncHttpServerUtils.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The HttpData class holds generic Http data
 *
 */
class HttpData
{
public:
    boost::optional<std::string> getHeader(std::string key);
    std::map<std::string, std::string> getHeader();
    std::string getContent();
    std::string getHttpVersion();
    std::string getEndpoint();
    AsyncHttpServerUtils::httpMethod getMethod();

protected:
    AsyncHttpServerUtils::httpMethod m_method;
    std::string m_version;
    std::map<std::string, std::string> m_header;
    std::string m_content;
    std::string m_endpoint;
};

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The HttpRequest class holds Http data for a request
 *
 */
class HttpRequest: public HttpData
{
public:
    HttpRequest(std::string rawRequest);
    boost::optional<std::string> getParam(std::string key);
    std::map<std::string, std::string> getParams();

    bool operator == (const AsyncHttpServerUtils::endpoint_t & endpoint);

private:
    std::map<std::string, std::string> m_params;
};

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The HttpResponse class holds Http data for a response
 *
 */
class HttpResponse: public HttpData
{
public:
    HttpResponse(AsyncHttpServerUtils::httpMethod method, std::string version, std::string endpoint);
    void addHeader(std::string key, std::string value);
    void setContent(std::string content);
    void setStatus(AsyncHttpServerUtils::responseStatusCode_t status);
    std::string encode();

private:
    AsyncHttpServerUtils::responseStatusCode_t m_status;
};

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The AsyncHttpConnectionInterfaces class Handles one connection to a client async
 *
 */
class AsyncHttpConnectionInterface: public AsyncTcpConnectionInterface
{
public:
    AsyncHttpConnectionInterface(boost::asio::io_context & ioContext, Plag * ptrParentPlag);

    void start();

    virtual HttpResponse workingRequest(HttpRequest req) = 0;

protected:
    std::string getRawRequest(); //!< function for getting the raw request from sock
};

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The AsyncHttpServer class declaration & implementation
 *
 */
template <class T>
class AsyncHttpServer: public AsyncTcpServer<T>
{
public:
    AsyncHttpServer(boost::asio::io_context & ioContext, Plag * ptrParentPla, uint16_t port = 80)
        AsyncTcpServer(ioContext, port, ptrParentPlag)
    {
    }
};

#endif /*ASYNCHTTPSERVER_HPP_*/