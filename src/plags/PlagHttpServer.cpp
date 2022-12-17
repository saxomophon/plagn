/**
 *-------------------------------------------------------------------------------------------------
 * @file PlagHttpServer.cpp
 * @author Fabian Köslin (fabian@koeslin.info)
 * @contributors:
 * @brief Implements the PlagHttpServer, PlagHttpServerTcpServer, PlagHttpServerConnection and Req/Res class
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

// std include
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

// own includes

// boost includes
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string_regex.hpp>

// self include
#include "PlagHttpServer.hpp"

using namespace std;
using namespace boost::asio;
using namespace boost::algorithm;
using ip::tcp;


PlagHttpServerConnection::PlagHttpServerConnection(boost::asio::io_context & ioContext, PlagHttpServer * ptrParentPlagHttpServer)
    : m_sock(ioContext),
    m_ptrParentPlagHttpServer(ptrParentPlagHttpServer)
{
    
}

PlagHttpServerConnection::pointer PlagHttpServerConnection::create(boost::asio::io_context & ioContext, PlagHttpServer * ptrParentPlagHttpServer)
{
    return pointer(new PlagHttpServerConnection(ioContext, ptrParentPlagHttpServer));
}

tcp::socket & PlagHttpServerConnection::socket()
{
    return m_sock;
}

int PlagHttpServerConnection::sendDatagramInterface(lua_State * L)
{
    // getting the table as map
    map<string, DataType> dgramMap;
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {   
            auto key = lua_tostring(L, -2);
            if (lua_isinteger(L, -1))
            {
                auto val = lua_tointeger(L, -1);
                dgramMap.insert(pair<string, int>(key, val));
            }
            else if (lua_isnumber(L, -1))
            {
                auto val = lua_tonumber(L, -1);
                dgramMap.insert(pair<string, double>(key, val));
            }
            else if (lua_isstring(L, -1))
            {
                auto val = lua_tostring(L, -1);
                dgramMap.insert(pair<string, string>(key, val));
            }
            lua_pop(L, 1);
        }
    }
    shared_ptr<DatagramHttpServer> dataToSend(
            new DatagramHttpServer(m_ptrParentPlagHttpServer->getName(), "", dgramMap));

    m_reqIds.push_back(to_string(m_ptrParentPlagHttpServer->sendDatagram(dataToSend)));

    return 0;
}

int PlagHttpServerConnection::resvDatagramInterface(lua_State * L)
{
    cout << "resvDatagramInterface()" << endl;
    const int waitingTime = 5000;
    auto start = std::chrono::high_resolution_clock::now();
    auto dgram = m_ptrParentPlagHttpServer->resvDatagram(m_reqIds);
        
    while (dgram  == nullptr)
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        if (elapsed.count() >= waitingTime)
        {
            break;
        }
        std::this_thread::sleep_for(1ms);
        dgram = m_ptrParentPlagHttpServer->resvDatagram(m_reqIds);
    }
    
    lua_newtable(L);
    if (dgram != nullptr)
    {
        for (auto param : dgram->getMap())
        {
            if (const int * pvar = get_if<int>(&param.second))
            {
                // value is int!
                lua_pushstring(L, param.first.c_str());
                lua_pushinteger(L, *pvar);
                lua_settable(L, -3);
            }
            else if (const unsigned int * pvar = get_if<unsigned int>(&param.second))
            {
                // value is uint!
                lua_pushstring(L, param.first.c_str());
                lua_pushinteger(L, *pvar);
                lua_settable(L, -3);
            }
            else if (const int64_t * pvar = get_if<int64_t>(&param.second))
            {
                // value is int64_t!
                lua_pushstring(L, param.first.c_str());
                lua_pushinteger(L, *pvar);
                lua_settable(L, -3);
            }
            else if (const uint64_t * pvar = get_if<uint64_t>(&param.second))
            {
                // value is uint64_t!
                lua_pushstring(L, param.first.c_str());
                lua_pushinteger(L, *pvar);
                lua_settable(L, -3);
            }
            else if (const double * pvar = get_if<double>(&param.second))
            {
                // value is double!
                lua_pushstring(L, param.first.c_str());
                lua_pushnumber(L, *pvar);
                lua_settable(L, -3);
            }
            else if (const string * pvar = get_if<string>(&param.second))
            {
                // value is string!
                lua_pushstring(L, param.first.c_str());
                lua_pushstring(L, pvar->c_str());
                lua_settable(L, -3);
            }
        }
    }
    
    return 1;
}

void PlagHttpServerConnection::start()
{
    PlagHttpServerHttpRequest req(getRawRequest());

    auto header = req.getHeader();
    auto params = req.getParams();

    cout << "Method: " << req.getMethod() << endl;
    cout << "URL: " << req.getEndpoint() << endl;
    cout << "Http Version: " << req.getHttpVersion() << endl;
    cout << "Complete Header: " << endl;
    for (auto headerPair : header)
    {
        cout << headerPair.first << ": " << headerPair.second << endl;
    }
    cout << "Params: " << endl;
    for (auto paramPair : params)
    {
        cout << paramPair.first << ": " << paramPair.second << endl;
    }
    cout << "Content: " << req.getContent() << endl;

    for (auto endpoint : m_ptrParentPlagHttpServer->m_endpoints)
    {
        if (req == endpoint)
        {
            // add static reqId to list
            m_reqIds.push_back(endpoint.endpoint);

            // Lua Magic
            // open the lua library
            lua_State * L = luaL_newstate();
            luaL_openlibs(L);

            // create header table
            lua_createtable(L, 0, 4);
            for (auto headerPair : req.getHeader())
            {
                lua_pushstring(L, headerPair.first.c_str());
                lua_pushstring(L, headerPair.second.c_str());
                lua_settable(L, -3);
            }
            lua_setglobal(L, "reqHeader");

            // create params table
            lua_createtable(L, 0, 4);
            for (auto headerPair : req.getParams())
            {
                lua_pushstring(L, headerPair.first.c_str());
                lua_pushstring(L, headerPair.second.c_str());
                lua_settable(L, -3);
            }
            lua_setglobal(L, "reqParams");

            // create global for content
            lua_pushstring(L, req.getContent().c_str());
            lua_setglobal(L, "reqContent");

            // create global for endpoint
            lua_pushstring(L, req.getEndpoint().c_str());
            lua_setglobal(L, "reqEndpoint");

            // create global for version
            lua_pushstring(L, req.getHttpVersion().c_str());
            lua_setglobal(L, "reqHttpVersion");

            // pushing the functions for dgram to lua
            // first store "this" in L's extra storage
            *static_cast<PlagHttpServerConnection **>(lua_getextraspace(L)) = this;
            // send dgram
            lua_pushcfunction(L, [](lua_State * L) -> int { 
                PlagHttpServerConnection * ptrThis = *static_cast<PlagHttpServerConnection **>(lua_getextraspace(L));
                return ptrThis->sendDatagramInterface(L);
            });
            lua_setglobal(L, "sendDatagram");
            // resv dgram
            lua_pushcfunction(L, [](lua_State * L) -> int {
                PlagHttpServerConnection * ptrThis = *static_cast<PlagHttpServerConnection **>(lua_getextraspace(L));
                return ptrThis->resvDatagramInterface(L);
            });
            lua_setglobal(L, "resvDatagram");

            // running the script
            if (luaL_dofile(L, "../docs/config/plags/plagHttpServer/example.lua") == LUA_OK)
            {
                cout << "[C] Executed example.lua\n";
                PlagHttpServerHttpResponse resp(req.getMethod(), req.getHttpVersion(), req.getEndpoint());
                
                lua_getglobal(L, "respHeader");
                if (lua_istable(L, -1))
                {
                    cout << endl << "respHeader:" << endl;
                    lua_pushnil(L);
                    while (lua_next(L, -2) != 0)
                    {
                        auto key = lua_tostring(L, -2);
                        auto val = lua_tostring(L, -1);
                        cout << key << ": " << val << endl;
                        resp.addHeader(key, val);
                        lua_pop(L, 1);
                    }
                }
                
                lua_getglobal(L, "respContent");
                auto content = lua_tostring(L, -1);
                cout << "respContent: " << content << endl;
                resp.setContent(content);


                lua_getglobal(L, "respStatus");
                if (lua_istable(L, -1))
                {
                    lua_getfield(L, -1, "code");
                    auto respCode = static_cast<int>(lua_tointeger(L, -1));

                    lua_getfield(L, -2, "message");
                    auto respMessage = lua_tostring(L, -1);

                    cout << "respCode: " << respCode << endl;
                    cout << "respMessage: " << respMessage << endl;
                    PlagHttpServer::responseStatusCode respStatus = { respCode, respMessage };
                    resp.setStatus(respStatus);
                }

                m_sock.write_some(boost::asio::buffer(resp.encode()));
                m_sock.close();
            }
            else
            {
                cout << "[C] Error reading script\n";
                PlagHttpServerHttpResponse resp(req.getMethod(), req.getHttpVersion(), req.getEndpoint());
                resp.setStatus(PlagHttpServer::HTTP_500);

                m_sock.write_some(boost::asio::buffer(resp.encode()));
                m_sock.close();
            }
            // close the Lua state
            lua_close(L);
        }
        else
        {
            PlagHttpServerHttpResponse resp(req.getMethod(), req.getHttpVersion(), req.getEndpoint());
            resp.setStatus(PlagHttpServer::HTTP_404);

            m_sock.write_some(boost::asio::buffer(resp.encode()));
            m_sock.close();
        }
    }
}

string PlagHttpServerConnection::getRawRequest()
{
    string retValue = "";
    bool nextRead = true;
    size_t contentLength = 0;

    do
    {
        const int bufSize = 1024;
        char buf[bufSize];
        memset(&buf[0], '\0', sizeof(buf));
        size_t len = m_sock.read_some(boost::asio::buffer(buf));
        
        retValue += string(buf, len);

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

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Const a new PlagHttpServerTcpServer::PlagHttpServerTcpServer object assigns default values
 *
 */
PlagHttpServerTcpServer::PlagHttpServerTcpServer(boost::asio::io_context & ioContext,
    uint16_t port, PlagHttpServer * ptrParentPlagHttpServer)
        : m_acceptor(ioContext, tcp::endpoint(tcp::v4(), port)),
        m_ioContext(ioContext),
        m_ptrParentPlagHttpServer(ptrParentPlagHttpServer)
{
    startAccept();
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagHttpServerTcpServer::startAccept accepts a new client connection
 *
 */
void PlagHttpServerTcpServer::startAccept()
{
 // socket
    PlagHttpServerConnection::pointer connection =
        PlagHttpServerConnection::create(m_ioContext, m_ptrParentPlagHttpServer);

   // asynchronous accept operation and wait for a new connection.
    m_acceptor.async_accept(connection->socket(),
       boost::bind(&PlagHttpServerTcpServer::handleAccept, this, connection,
           boost::asio::placeholders::error));
}


/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagHttpServerTcpServer::handleAccept handles a new client connection
 *
 */
void PlagHttpServerTcpServer::handleAccept(PlagHttpServerConnection::pointer connection, const boost::system::error_code & err)
{
    if (!err)
    {
        connection->start();
    }
    startAccept();
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Plag HttpServer:: Plag HttpServer object assigns default values
 * 
 */
PlagHttpServer::PlagHttpServer(const boost::property_tree::ptree & propTree,
                 const std::string & name, const uint64_t & id) :
    Plag(propTree, name, id, PlagType::HttpServer)
{
    readConfig();
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Destroy the Plag Udp:: Plag Udp object closes socket, in addition to regular destruct
 * 
 */
PlagHttpServer::~PlagHttpServer()
{
    if (!m_stopToken) stopWork();
    try
    {
        m_ioContext.stop();
        // wait until the ioService is stopped
        while (!m_ioContext.stopped()) {}
    }
    catch (exception & e)
    {
        cerr << "Could not close PlagHttpServer, because of " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Could not close PlagHttpServer, because for unknown reason!" << endl;
    }
}

void PlagHttpServer::readConfig() try
{
    m_port = getParameter<uint16_t>("port");

    size_t idx = 1;

    
    while (getOptionalParameter<string>("endpoint[" + to_string(idx) + "].endpoint", "") != "" &&
           getOptionalParameter<string>("endpoint[" + to_string(idx) + "].method", "") != "" &&
           getOptionalParameter<string>("endpoint[" + to_string(idx) + "].scriptFile", "") != "")
    {

        endpoint tmp;
        tmp.endpoint = getOptionalParameter<string>("endpoint[" + to_string(idx) + "].endpoint", "");
        string strMethod = getOptionalParameter<string>("endpoint[" + to_string(idx) + "].method", "");
        strMethod = to_upper_copy(strMethod);

        if (strMethod == "POST")
        {
            tmp.method = HTTP_POST;
        }
        else if (strMethod == "PUT")
        {
            tmp.method = HTTP_PUT;
        }
        else if (strMethod == "GET")
        {
            tmp.method = HTTP_GET;
        }
        else if (strMethod == "DELETE")
        {
            tmp.method = HTTP_DELETE;
        }
        else if (strMethod == "HEAD")
        {
            tmp.method = HTTP_HEAD;
        }
        else if (strMethod == "CONNECT")
        {
            tmp.method = HTTP_CONNECT;
        }
        else if (strMethod == "OPTIONS")
        {
            tmp.method = HTTP_OPTIONS;
        }
        else if (strMethod == "TRACE")
        {
            tmp.method = HTTP_TRACE;
        }
        else if (strMethod == "PATCH")
        {
            tmp.method = HTTP_PATCH;
        }
        else 
        {
            tmp.method = HTTP_UNKNOWN;
        }

        tmp.scriptFile = getOptionalParameter<string>("endpoint[" + to_string(idx) + "].scriptFile", "");
        tmp.workingDirectory = getOptionalParameter<string>("endpoint[" + to_string(idx) + "].workingDirectory", "./");
        m_endpoints.push_back(tmp);
        idx++;
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagHttpServer::readConfig()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagHttpServer::init() 
 * 
 */
void PlagHttpServer::init() try
{
    m_tcpServer = shared_ptr<PlagHttpServerTcpServer>(new PlagHttpServerTcpServer(m_ioContext, m_port, this));
    m_ioContextThread = shared_ptr<thread>(new thread([=]() {
        this->m_ioContext.run();
    }));
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagHttpServer::init()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagHttpServer::loopWork regularly reads on the socket, if data popped in or sends data
 * 
 */
bool PlagHttpServer::loopWork() try
{
    // we do not need to do any work here :-)
    return true;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened during PlagHttpServer::loopWork()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief placeDatagram is a function to place a Datagram here. Hence this method decodes the
 * datagram
 *
 * @param datagram A Datagram containing data for this Plag to interprete
 */
void PlagHttpServer::placeDatagram(const shared_ptr<Datagram> datagram) try
{
    const shared_ptr<DatagramHttpServer> castPtr = dynamic_pointer_cast<DatagramHttpServer>(datagram);
    if (castPtr != nullptr)
    {
        for (auto it = m_endpoints.begin(); it != m_endpoints.end(); it++)
        {
            if (it->endpoint == castPtr->getReqId())
            {
                it->stateDgram = castPtr;
                return;
            }
        }
        m_incommingDatagrams.push_back(datagram);
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagHttpServer::placeDatagram()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

boost::optional<string> PlagHttpServerHttpData::getHeader(string key)
{
    if (m_header.count(key) == 1)
    {
        return m_header[key];
    }

    return boost::none;
}

map<string, string> PlagHttpServerHttpData::getHeader()
{
    return m_header;
}

std::string PlagHttpServerHttpData::getContent() 
{
    return m_content;
}

std::string PlagHttpServerHttpData::getHttpVersion()
{
    return m_version;
}

std::string PlagHttpServerHttpData::getEndpoint()
{
    return m_endpoint;
}

PlagHttpServer::httpMethod PlagHttpServerHttpData::getMethod()
{
    return m_method;
}

int PlagHttpServer::sendDatagram(std::shared_ptr<DatagramHttpServer> dgram)
{
    const lock_guard<mutex> lock(m_mtxSending);
    
    static int reqId = 0;

    dgram->setReqId(to_string(reqId));
    
    appendToDistribution(dgram);

    return reqId++;
}

std::shared_ptr<DatagramHttpServer> PlagHttpServer::resvDatagram(const vector<string> & reqIds)
{
    const lock_guard<mutex> lock(m_mtxRecv);

    for (auto dgramIt = m_incommingDatagrams.begin();
                dgramIt != m_incommingDatagrams.end(); dgramIt++)
    {
        const shared_ptr<DatagramHttpServer> castPtr = dynamic_pointer_cast<DatagramHttpServer>(*dgramIt);
        for (auto reqId : reqIds)
        {
            if (reqId == castPtr->getReqId())
            {
                m_incommingDatagrams.erase(dgramIt);
                return castPtr;
            }
        }
    }

    for (auto endpoint : m_endpoints)
    {
        for (auto reqId : reqIds)
        {
            if (endpoint.endpoint == reqId)
            {
                return endpoint.stateDgram;
            }
        }
    }
    
    return nullptr;
}

// Http Response Codes and Messages 
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_100 = { 100, "Continue" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_101 = { 101, "Switching Protocols" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_103 = { 103, "Early Hints" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_200 = { 200, "OK" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_201 = { 201, "Created" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_202 = { 202, "Accepted" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_203 = { 203, "Non - Authoritative Information" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_204 = { 204, "No Content" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_205 = { 205, "Reset Content" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_206 = { 206, "Partial Content" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_300 = { 300, "Multiple Choices" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_301 = { 301, "Moved Permanently" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_302 = { 302, "Found" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_303 = { 303, "See Other" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_304 = { 304, "Not Modified" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_307 = { 307, "Temporary Redirect" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_308 = { 308, "Permanent Redirect" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_400 = { 400, "Bad Request" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_401 = { 401, "Unauthorized" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_402 = { 402, "Payment Required" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_403 = { 403, "Forbidden" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_404 = { 404, "Not Found" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_405 = { 405, "Method Not Allowed" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_406 = { 406, "Not Acceptable" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_407 = { 407, "Proxy Authentication Required" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_408 = { 408, "Request Timeout" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_409 = { 409, "Conflict" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_410 = { 410, "Gone" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_411 = { 411, "Length Required" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_412 = { 412, "Precondition Failed" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_413 = { 413, "Payload Too Large" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_414 = { 414, "URI Too Long" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_415 = { 415, "Unsupported Media Type" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_416 = { 416, "Range Not Satisfiable" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_417 = { 417, "Expectation Failed" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_418 = { 418, "I'm a teapot" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_425 = { 425, "Too Early" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_426 = { 426, "Upgrade Required" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_428 = { 428, "Precondition Required" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_429 = { 429, "Too Many Requests" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_431 = { 431, "Request Header Fields Too Large" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_451 = { 451, "Unavailable For Legal Reasons" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_500 = { 500, "Internal Server Error" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_501 = { 501, "Not Implemented" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_502 = { 502, "Bad Gateway" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_503 = { 503, "Service Unavailable" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_504 = { 504, "Gateway Timeout" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_505 = { 505, "HTTP Version Not Supported" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_506 = { 506, "Variant Also Negotiates" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_510 = { 510, "Not Extended" };
const PlagHttpServer::responseStatusCode PlagHttpServer::HTTP_511 = { 511, "Network Authentication Required" };

PlagHttpServerHttpRequest::PlagHttpServerHttpRequest(std::string rawRequest)
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
        m_method = PlagHttpServer::HTTP_GET;
    }
    else if (to_upper_copy(firstRow[0]) == "POST")
    {
        m_method = PlagHttpServer::HTTP_POST;
    }
    else if (to_upper_copy(firstRow[0]) == "PUT")
    {
        m_method = PlagHttpServer::HTTP_PUT;
    }
    else if (to_upper_copy(firstRow[0]) == "DELETE")
    {
        m_method = PlagHttpServer::HTTP_DELETE;
    }
    else if (to_upper_copy(firstRow[0]) == "TRACE")
    {
        m_method = PlagHttpServer::HTTP_TRACE;
    }
    else if (to_upper_copy(firstRow[0]) == "HEAD")
    {
        m_method = PlagHttpServer::HTTP_HEAD;
    }
    else if (to_upper_copy(firstRow[0]) == "CONNECT")
    {
        m_method = PlagHttpServer::HTTP_CONNECT;
    }
    else if (to_upper_copy(firstRow[0]) == "OPTIONS")
    {
        m_method = PlagHttpServer::HTTP_OPTIONS;
    }
    else if (to_upper_copy(firstRow[0]) == "PATCH")
    {
        m_method = PlagHttpServer::HTTP_PATCH;
    }
    else
    {
        m_method = PlagHttpServer::HTTP_UNKNOWN;
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

boost::optional<std::string> PlagHttpServerHttpRequest::getParam(std::string key)
{
    if (m_params.count(key) == 1)
    {
        return m_params[key];
    }

    return boost::none;
}

std::map<std::string, std::string> PlagHttpServerHttpRequest::getParams()
{
    return m_params;
}

bool PlagHttpServerHttpRequest::operator == (const PlagHttpServer::endpoint & endpoint)
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

PlagHttpServerHttpResponse::PlagHttpServerHttpResponse(PlagHttpServer::httpMethod method, string version, string endpoint)
{
    m_method = method;
    m_version = version;
    m_endpoint = endpoint;

    addHeader("Server", "Plagn/0.0.1");
}

void PlagHttpServerHttpResponse::addHeader(string key, string value)
{
    m_header.insert(pair<string, string>(key, value));
}

void PlagHttpServerHttpResponse::setContent(string content)
{
    m_content = content;
}

void PlagHttpServerHttpResponse::setStatus(PlagHttpServer::responseStatusCode status)
{
    m_status = status;
}

string PlagHttpServerHttpResponse::encode() 
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