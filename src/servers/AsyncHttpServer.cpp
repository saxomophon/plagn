/**
 *-------------------------------------------------------------------------------------------------
 * @file AsyncHttpServer.cpp
 * @author Fabian Köslin
 * @contributors:
 * @brief Implements the AsyncTcpServer class
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

// std includes

// self include
#include "AsyncHttpServer.hpp"

// boost includes
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string_regex.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::algorithm;
using namespace AsyncHttpServerUtils;

boost::optional<string> HttpData::getHeader(string key)
{
    if (m_header.count(key) == 1)
    {
        return m_header[key];
    }

    return boost::none;
}

map<string, string> HttpData::getHeader()
{
    return m_header;
}

std::string HttpData::getContent()
{
    return m_content;
}

std::string HttpData::getHttpVersion()
{
    return m_version;
}

std::string HttpData::getEndpoint()
{
    return m_endpoint;
}

httpMethod HttpData::getMethod()
{
    return m_method;
}

HttpRequest::HttpRequest(std::string rawRequest)
{
    // parsing the rawRequest
    // get the content and the header;
    vector<string> headerAndContent;
    split_regex(headerAndContent, rawRequest, boost::regex("\x0D\x0A\x0D\x0A"));

    if (headerAndContent.size() >= 2)
    {
        m_content = headerAndContent[1];
    }
    else
    {
        m_content = "";
    }

    vector<string> header;
    split_regex(header, headerAndContent[0], boost::regex("\x0D\x0A"));

    vector<string> firstRow;
    split(firstRow, header[0], is_any_of(" "));

    // parsing the http method
    if (to_upper_copy(firstRow[0]) == "GET")
    {
        m_method = httpMethod::HTTP_GET;
    }
    else if (to_upper_copy(firstRow[0]) == "POST")
    {
        m_method = httpMethod::HTTP_POST;
    }
    else if (to_upper_copy(firstRow[0]) == "PUT")
    {
        m_method = httpMethod::HTTP_PUT;
    }
    else if (to_upper_copy(firstRow[0]) == "DELETE")
    {
        m_method = httpMethod::HTTP_DELETE;
    }
    else if (to_upper_copy(firstRow[0]) == "TRACE")
    {
        m_method = httpMethod::HTTP_TRACE;
    }
    else if (to_upper_copy(firstRow[0]) == "HEAD")
    {
        m_method = httpMethod::HTTP_HEAD;
    }
    else if (to_upper_copy(firstRow[0]) == "CONNECT")
    {
        m_method = httpMethod::HTTP_CONNECT;
    }
    else if (to_upper_copy(firstRow[0]) == "OPTIONS")
    {
        m_method = httpMethod::HTTP_OPTIONS;
    }
    else if (to_upper_copy(firstRow[0]) == "PATCH")
    {
        m_method = httpMethod::HTTP_PATCH;
    }
    else
    {
        m_method = httpMethod::HTTP_UNKNOWN;
    }

    vector<string> urlParams;
    split(urlParams, firstRow[1], is_any_of("?"));
    m_endpoint = urlParams[0];

    if (urlParams.size() > 1)
    {
        vector<string> params;
        split(params, urlParams[1], is_any_of("&"));
        for (auto param : params)
        {
            vector<string> paramPair;
            split(paramPair, param, is_any_of("="));
            m_params.insert(pair<string, string>(paramPair[0], paramPair[1]));
        }
    }

    m_version = firstRow[2];

    // following header:
    for (int i = 1; i < header.size(); i++)
    {
        vector<string> headerPair;
        split_regex(headerPair, header[i], boost::regex(": "));
        m_header.insert(pair<string, string>(headerPair[0], headerPair[1]));
    }
}

boost::optional<std::string> HttpRequest::getParam(std::string key)
{
    if (m_params.count(key) == 1)
    {
        return m_params[key];
    }

    return boost::none;
}

std::map<std::string, std::string> HttpRequest::getParams()
{
    return m_params;
}

bool HttpRequest::operator == (const endpoint_t & endpoint)
{
    // check the HTTP_METHOD
    if (m_method != endpoint.method)
        return false;

    // check Http Endpoint
    if (contains(endpoint.endpoint, "*"))
    {
        auto pos = endpoint.endpoint.find('*');
        return endpoint.endpoint.substr(0, pos) == m_endpoint.substr(0, pos);
    }
    else
    {
        return endpoint.endpoint == m_endpoint;
    }

    return false;
}

HttpResponse::HttpResponse(httpMethod method, string version, string endpoint)
{
    m_method = method;
    m_version = version;
    m_endpoint = endpoint;

    addHeader("Server", "Plagn/0.0.1");
}

void HttpResponse::addHeader(string key, string value)
{
    m_header.insert(pair<string, string>(key, value));
}

void HttpResponse::setContent(string content)
{
    m_content = content;
}

void HttpResponse::setStatus(responseStatusCode_t status)
{
    m_status = status;
}

string HttpResponse::encode()
{
    string encoded = m_version + " " + to_string(m_status.code) + " " + m_status.message + "\x0D\x0A";
    for (auto headerPair : m_header)
    {
        encoded += headerPair.first + ": " + headerPair.second + "\x0D\x0A";
    }
    encoded += "Content-Length: " + to_string(m_content.length()) + "\x0D\x0A";
    encoded += "\x0D\x0A";
    encoded += m_content;

    return encoded;
}

AsyncHttpConnectionInterface::AsyncHttpConnectionInterface(boost::asio::io_context & ioContext, Plag * ptrParentPlag)
    : AsyncTcpConnectionInterface(ioContext, ptrParentPlag)
{
}

string AsyncHttpConnectionInterface::getRawRequest()
{
    string retValue = "";
    bool nextRead = true;
    size_t contentLength = 0;

    do
    {
        retValue += readSome();

        if (contains(to_lower_copy(retValue), "content-length: ") && contentLength == 0)
        {
            size_t pos = to_lower_copy(retValue).find("content-length: ");

            if (pos != string::npos)
            {
                pos += sizeof("content-length: ");
                auto posStart = pos - 1;
                while (retValue[pos] != '\x0D' && pos < retValue.length())
                {
                    pos++;
                }

                if (pos < retValue.length())
                {
                    contentLength = stoi(retValue.substr(posStart, pos - posStart));
                }
            }
        }

        if (contains(retValue, "\x0D\x0A\x0D\x0A"))
        {
            size_t pos = retValue.find("\x0D\x0A\x0D\x0A");
            pos += sizeof("\x0D\x0A\x0D\x0A") - 1;

            if (retValue.length() - pos == contentLength)
            {
                nextRead = false;
            }
            else if (retValue.length() - pos > contentLength)
            {
                retValue = retValue.substr(0, pos + contentLength);
                nextRead = false;
            }
        }

    } while (nextRead);

    return retValue;
}

void AsyncHttpConnectionInterface::start()
{
    HttpResponse resp = workingRequest(HttpRequest(getRawRequest()));

    writeSome(resp.encode());

    closeSock();
}