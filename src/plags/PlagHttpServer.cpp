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


PlagHttpServerConnection::PlagHttpServerConnection(boost::asio::io_context & ioContext, Plag * ptrParentPlag)
    : AsyncHttpConnectionInterface(ioContext, ptrParentPlag)
{
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

HttpResponse PlagHttpServerConnection::workingRequest(HttpRequest req)
{
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

    auto castPtrParent = dynamic_cast<PlagHttpServer*>(m_ptrParentPlag);

    for (auto endpoint : castPtrParent->m_endpoints)
    {
        if (req == endpoint.endpointDef)
        {
            // add static reqId to list
            m_reqIds.push_back(endpoint.endpointDef.endpoint);

            // Lua Magic
            LuaWrapper LuaWrapper;

            // create header table
            LuaWrapper.createTable("reqHeader", req.getHeader());

            // create params table
            luaWrapper.createTable("reqParams", req.getParams());

            // create global for content
            luaWrapper.createString("reqContent", req.getContent());

            // create global for endpoint
            luaWrapper.createString("reqEndpoint", req.getEndpoint());

            // create global for version
            luaWrapper.createString("reqHttpVersion", req.getHttpVersion());

            // pushing the functions for dgram to lua
            // first store "this" in L's extra storage
            luaWrapper.addObjectPtr<PlagHttpServerConnection>(this);
            
            // send dgram
            luaWrapper.addObjectPtr<PlagHttpServerConnection>("sendDatagram", [](lua_State * L) -> int
            {
                PlagHttpServerConnection * ptrThis = LuaWrapper::getObjectPtr<PlagHttpServerConnection>(L);
                return ptrThis->sendDatagramInterface(L);
            });
            // resv dgram
            luaWrapper.addObjectPtr<PlagHttpServerConnection>("resvDatagram", [](lua_State * L) -> int
            {
                PlagHttpServerConnection * ptrThis = LuaWrapper::getObjectPtr<PlagHttpServerConnection>(L);
                return ptrThis->resvDatagramInterface(L);
            });

            // running the script
            if (luaWrapper.executeFile("../docs/config/plags/plagHttpServer/example.lua") == LUA_OK)
            {
                cout << "[C] Executed example.lua\n";
                HttpResponse resp(req.getMethod(), req.getHttpVersion(), req.getEndpoint());
                
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
        tmp.endpointDef.endpoint = getOptionalParameter<string>("endpoint[" + to_string(idx) + "].endpoint", "");
        string strMethod = getOptionalParameter<string>("endpoint[" + to_string(idx) + "].method", "");
        strMethod = to_upper_copy(strMethod);

        if (strMethod == "POST")
        {
            tmp.endpointDef.method = AsyncHttpServerUtils::HTTP_POST;
        }
        else if (strMethod == "PUT")
        {
            tmp.endpointDef.method = AsyncHttpServerUtils::HTTP_PUT;
        }
        else if (strMethod == "GET")
        {
            tmp.endpointDef.method = AsyncHttpServerUtils::HTTP_GET;
        }
        else if (strMethod == "DELETE")
        {
            tmp.endpointDef.method = AsyncHttpServerUtils::HTTP_DELETE;
        }
        else if (strMethod == "HEAD")
        {
            tmp.endpointDef.method = AsyncHttpServerUtils::HTTP_HEAD;
        }
        else if (strMethod == "CONNECT")
        {
            tmp.endpointDef.method = AsyncHttpServerUtils::HTTP_CONNECT;
        }
        else if (strMethod == "OPTIONS")
        {
            tmp.endpointDef.method = AsyncHttpServerUtils::HTTP_OPTIONS;
        }
        else if (strMethod == "TRACE")
        {
            tmp.endpointDef.method = AsyncHttpServerUtils::HTTP_TRACE;
        }
        else if (strMethod == "PATCH")
        {
            tmp.endpointDef.method = AsyncHttpServerUtils::HTTP_PATCH;
        }
        else 
        {
            tmp.endpointDef.method = AsyncHttpServerUtils::HTTP_UNKNOWN;
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
    m_tcpServer = shared_ptr<AsyncHttpServer<PlagHttpServerConnection>>(new AsyncHttpServer<PlagHttpServerConnection>(m_ioContext, this, m_port));
    m_ioContextThread = shared_ptr<thread>(new thread([this]() {
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