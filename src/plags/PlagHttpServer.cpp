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

/**
 * -------------------------------------------------------------------------------------------------
 * @brief constructs a PlagHttpServerConnection
 *
 * @param ioContext The io context
 * @param ptrParentPlag The parent plag
*/
PlagHttpServerConnection::PlagHttpServerConnection(boost::asio::io_context & ioContext, Plag * ptrParentPlag)
    : AsyncHttpConnectionInterface(ioContext, ptrParentPlag)
{
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief sendDatagramInterface Sends a datagram to the plagn infrastructure
 *
 * @param L The lua state
 * @return int The number of return values
*/
int PlagHttpServerConnection::sendDatagramInterface(lua_State * L)
{
    // casting the parent plag
    auto castParentPtr = dynamic_cast<PlagHttpServer *>(m_ptrParentPlag);
    
    // getting the table as map
    auto dgramMap = LuaWrapper::getTable(L, -1);

    // building the dgram
    shared_ptr<DatagramHttpServer> dataToSend(
            new DatagramHttpServer(castParentPtr->getName(), "", dgramMap));

    // send the dgram and stored the 
    m_reqIds.push_back(to_string(castParentPtr->sendDatagram(dataToSend)));

    return 0;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief resvDatagramInterface Receives a datagram from the plagn infrastructure
 *
 * @param L The lua state
 * @return int The number of return values
*/
int PlagHttpServerConnection::resvDatagramInterface(lua_State * L)
{
    // casting the parent plag
    auto castParentPtr = dynamic_cast<PlagHttpServer *>(m_ptrParentPlag);

    // some consts and variables
    const int waitingTime = 5000; // TODO: Make this a paramter
    auto start = std::chrono::high_resolution_clock::now();
    auto dgram = castParentPtr->resvDatagram(m_reqIds);
        
    while (dgram  == nullptr)
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        if (elapsed.count() >= waitingTime)
        {
            break;
        }
        std::this_thread::sleep_for(1ms);
        dgram = castParentPtr->resvDatagram(m_reqIds);
    }
    
    if (dgram != nullptr)
    {
        LuaWrapper::createTable(L, dgram->getMap());
    }
    else 
    {
        LuaWrapper::createTable(L, map<string, DataType>());
    }
    
    return 1;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief workingRequest Handles the request
 *
 * @param req The request
 * @return HttpResponse The response
*/
HttpResponse PlagHttpServerConnection::workingRequest(HttpRequest req)
{
    auto header = req.getHeader();
    auto params = req.getParams();

    auto castPtrParent = dynamic_cast<PlagHttpServer*>(m_ptrParentPlag);

    for (auto endpoint : castPtrParent->m_endpoints)
    {
        if (req == endpoint.endpointDef)
        {
            // add static reqId to list
            m_reqIds.push_back(endpoint.endpointDef.endpoint);

            // Lua Magic
            LuaWrapper luaWrapper;

            // create header table
            luaWrapper.createTable("reqHeader", req.getHeader());

            // create params table
            luaWrapper.createTable("reqParams", req.getParams());

            // create global for content
            luaWrapper.createString("reqContent", req.getContent());

            // create global for endpoint
            luaWrapper.createString("reqEndpoint", req.getEndpoint());

            // create global for version
            luaWrapper.createString("reqHttpVersion", req.getHttpVersion());

            // create global wd
            luaWrapper.createString("reqWorkingDir", endpoint.workingDirectory);

            // pushing the functions for dgram to lua
            // first store "this" in L's extra storage
            luaWrapper.addObjectPtr<PlagHttpServerConnection>(this);
            
            // send dgram
            luaWrapper.createFunction("sendDatagram", [](lua_State * L) -> int
            {
                auto ptrThis = LuaWrapper::getObjectPtr<PlagHttpServerConnection>(L);
                return ptrThis->sendDatagramInterface(L);
            });
            
            // resv dgram
            luaWrapper.createFunction("resvDatagram", [](lua_State * L) -> int
            {
                auto ptrThis = LuaWrapper::getObjectPtr<PlagHttpServerConnection>(L);
                return ptrThis->resvDatagramInterface(L);
            });

            // running the script
            if (luaWrapper.executeFile(endpoint.scriptFile) == LUA_OK)
            {
                HttpResponse resp(req.getMethod(), req.getHttpVersion(), req.getEndpoint());

                resp.addHeader(luaWrapper.getTableS("respHeader"));

                resp.setContent(luaWrapper.getString("respContent"));

                auto respStatusMap = luaWrapper.getTableS("respStatus");
                
                auto code = stoi(respStatusMap["code"]);
                auto msg = respStatusMap["message"];
                resp.setStatus(AsyncHttpServerUtils::responseStatusCode_t({ code, msg }));

                return resp;
            }
            else
            {
                cout << "[C] Error reading script: " << endpoint.scriptFile << endl;
                HttpResponse resp(req.getMethod(), req.getHttpVersion(), req.getEndpoint());
                resp.setStatus(AsyncHttpServerUtils::HTTP_500);

                return resp;
            }
        }
        else
        {
            HttpResponse resp(req.getMethod(), req.getHttpVersion(), req.getEndpoint());
            resp.setStatus(AsyncHttpServerUtils::HTTP_404);

            return resp;
        }
    }
    HttpResponse resp(req.getMethod(), req.getHttpVersion(), req.getEndpoint());
    resp.setStatus(AsyncHttpServerUtils::HTTP_404);

    return resp;
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

/**
 *-------------------------------------------------------------------------------------------------
 * @brief readConfig reads the config file and assigns the values to the member variables
 * 
 */
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
            if (it->endpointDef.endpoint == castPtr->getReqId())
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

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagHttpServer::sendDatagram sends a Datagram to the next Plag in the chain
 * 
 * @param dgram The Datagram to send
 * @return int The id of the Datagram
 */
int PlagHttpServer::sendDatagram(std::shared_ptr<DatagramHttpServer> dgram)
{
    const lock_guard<mutex> lock(m_mtxSending);
    
    static int reqId = 0;

    dgram->setReqId(to_string(reqId));
    
    appendToDistribution(dgram);

    return reqId++;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagHttpServer::resvDatagram receives a Datagram from the previous Plag in the chain
 * 
 * @param reqIds The ids of the Datagram to receive
 * @return std::shared_ptr<DatagramHttpServer> The Datagram
 */
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
            if (endpoint.endpointDef.endpoint == reqId)
            {
                return endpoint.stateDgram;
            }
        }
    }
    
    return nullptr;
}