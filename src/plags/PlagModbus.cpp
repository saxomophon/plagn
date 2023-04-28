/**
 *-------------------------------------------------------------------------------------------------
 * @file PlagModbus.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements the PlagModbus class
 * @version 0.1
 * @date 2023-04-26
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

// boost includes
#include <boost/algorithm/string.hpp>

// own includes

// self include
#include "PlagModbus.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new PlagModbus::PlagModbus object assigns default values
 *
 */
PlagModbus::PlagModbus(const boost::property_tree::ptree & propTree,
                   const std::string & name, const uint64_t & id):
    Plag(propTree, name, id, PlagType::MQTT)
{
    readConfig();
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Destroy the PlagModbus::PlagModbus object closes socket, in addition to regular destruct
 *
 */
PlagModbus::~PlagModbus()
{
    if (!m_stopToken) stopWork();
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief reads many optional parameters from config and assigns them to member values
 *
 */
void PlagModbus::readConfig() try
{
    m_connectionType = getOptionalParameter<string>("type", "tcpClient");

    m_usesSerial = (m_connectionType == "serialClient" || m_connectionType == "sc"
                    || m_connectionType == "serialServer" || m_connectionType == "ss");

    // parameters for serial connections
    if (m_usesSerial)
    {
        m_comPort = getParameter<string>("comPort");
        m_baudRate = getOptionalParameter<uint32_t>("baud", 19200);
        m_parity = getOptionalParameter<string>("parity", "none");
        m_byteSize = getOptionalParameter<uint8_t>("byteSize", 8);
        m_stopBits = getOptionalParameter<uint8_t>("stopBits", 10);
    }
    else
    {
        m_serverIP = getOptionalParameter<string>("serverIP", "127.0.0.1");
        m_tcpPort = getParameter<uint16_t>("port");
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagModbus::readConfig()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagModbus::init() configures the MQTT interface
 *
 */
void PlagModbus::init() try
{
    if (m_connectionType == "serialClient"
        || m_connectionType == "sc"
        || m_connectionType == "serialServer"
        || m_connectionType == "ss")
    {
        //TODO: create serial layer
    }
    else if (m_connectionType == "tcpClient"
             || m_connectionType == "tc")
    {
        m_dataLayer = make_unique<TcpClient>(1000, *this, m_serverIP, m_tcpPort);
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagModbus::init()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief PlagModbus::loopWork regularly reads on the socket, if data popped in or sends data
 *
 */
bool PlagModbus::loopWork() try
{
    bool somethingDone = false;
    if (m_dataLayer->getAvailableBytesCount() > 0)
    {
        m_buffer = m_dataLayer->receiveBytes(0);
        somethingDone = true;
    }
    if (m_incommingDatagrams.size() > 0)
    {
        std::shared_ptr<DatagramModbus> datagram;
        datagram = dynamic_pointer_cast<DatagramModbus>(m_incommingDatagrams.front());
        string dataToSend = makeBytesFromDatagram(datagram);
        m_dataLayer->transmit(dataToSend);
        m_incommingDatagrams.pop_front();
        somethingDone = true;
    }
    return somethingDone;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened during PlagModbus::loopWork()";
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
void PlagModbus::placeDatagram(const shared_ptr<Datagram> datagram) try
{
    const shared_ptr<DatagramModbus> castPtr = dynamic_pointer_cast<DatagramModbus>(datagram);
    if (castPtr != nullptr)
    {
        m_incommingDatagrams.push_back(datagram);
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagModbus::placeDatagram()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

void PlagModbus::parseBuffer() try
{
    string pdu = extractPDU();
    if (pdu.size() > 3)
    {
        uint8_t functionCode = static_cast<uint8_t>(pdu[0]);
        bool isError = functionCode & FunctionCode::EXCEPTION_MASK;
        switch (functionCode)
        {
        case FunctionCode::READ_COIL:
            break;
        case FunctionCode::READ_INPUT:
            break;
        case FunctionCode::READ_HOLDING_REGISTER:
            break;
        case FunctionCode::READ_INPUT_REGISTER:
            break;
        case FunctionCode::WRITE_SINGLE_COIL:
            break;
        case FunctionCode::WRITE_SINGLE_REGISTER:
            break;
        case FunctionCode::READ_EXCEPTION:
            break;
        case FunctionCode::DIAGNOSTICS:
            break;
        case FunctionCode::GET_COM_COUNTER:
            break;
        case FunctionCode::GET_COM_LOG:
            break;
        case FunctionCode::WRITE_COILS:
            break;
        case FunctionCode::WRITE_REGISTERS:
            break;
        case FunctionCode::REPORT_SERVER_ID:
            break;
        case FunctionCode::READ_FILE:
            break;
        case FunctionCode::WRITE_FILE:
            break;
        case FunctionCode::MASK_WRITE_REGISTER:
            break;
        case FunctionCode::READ_WRITE_REGISTERS:
            break;
        case FunctionCode::READ_FIFO:
            break;
        case FunctionCode::ENCAPSULATED:
            // currently not supported
            break;
        default:
            if (isError)
            {
                cout << "Received error on function code: " << functionCode << endl;
            }
            else
            {
                throw std::invalid_argument("Received unknonwn function code!");
            }
        }
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagModbus::parseBuffer()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

string PlagModbus::extractPDU() try
{
    string extraction = "";
    uint8_t id = 0;
    if (m_buffer.size() > 3)
    {
        id = static_cast<uint8_t>(m_buffer[0]);
        uint8_t functionCode = static_cast<uint8_t>(m_buffer.at(m_usesSerial ? 1 : 0));
        bool isError = (functionCode & FunctionCode::EXCEPTION_MASK) > 0;
        switch (functionCode)
        {
        case FunctionCode::READ_COIL: // deliberate fall-through
        case FunctionCode::READ_INPUT:
            if (m_isServer)
            {
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, 7);
            }
            else
            {
                uint8_t byteCount = static_cast<uint8_t>(m_buffer.at(m_usesSerial ? 2 : 1));
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, byteCount + 4);
            }
            break;
        case FunctionCode::READ_HOLDING_REGISTER: // deliberate fall-through
        case FunctionCode::READ_INPUT_REGISTER:
            if (m_isServer)
            {
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, 7);
            }
            else
            {
                uint8_t byteCount = static_cast<uint8_t>(m_buffer.at(m_usesSerial ? 2 : 1));
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, byteCount * 2 + 4);
            }
            break;
        case FunctionCode::WRITE_SINGLE_COIL: // deliberate fall-through
        case FunctionCode::WRITE_SINGLE_REGISTER:
            extraction = m_buffer.substr(m_usesSerial ? 1 : 0, 7);
            break;
        case FunctionCode::READ_EXCEPTION:
            extraction = m_buffer.substr(1, m_isServer ? 5 : 6);
            break;
        case FunctionCode::DIAGNOSTICS:
            // TODO: work through this
            cout << "Resetting buffer, because of diagnostics request" << endl;
            m_buffer = "";
            break;
        case FunctionCode::GET_COM_COUNTER:
            extraction = m_buffer.substr(1, m_isServer ? 3 : 7);
            break;
        case FunctionCode::GET_COM_LOG:
            if (m_isServer)
            {
                extraction = m_buffer.substr(1, 3);
            }
            else
            {

                uint8_t byteCount = static_cast<uint8_t>(m_buffer.at(2));
                extraction = m_buffer.substr(1, byteCount + 4);
            }
            break;
        case FunctionCode::WRITE_COILS:
            if (m_isServer)
            {
                uint8_t byteCount = static_cast<uint8_t>(m_buffer.at(m_usesSerial ? 6 : 5));
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, byteCount + 8);
            }
            else
            {
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, 7);
            }
            break;
        case FunctionCode::WRITE_REGISTERS:
            if (m_isServer)
            {
                uint8_t byteCount = static_cast<uint8_t>(m_buffer.at(m_usesSerial ? 6 : 5));
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, byteCount * 2 + 8);
            }
            else
            {
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, 7);
            }
            break;
        case FunctionCode::REPORT_SERVER_ID:
            if (m_isServer)
            {
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, 1);
            }
            else
            {
                uint8_t byteCount = static_cast<uint8_t>(m_buffer.at(m_usesSerial ? 2 : 1));
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, byteCount + 4);
            }
            break;
        case FunctionCode::READ_FILE:   // deliberate fall-through
        case FunctionCode::WRITE_FILE:
            {
                uint8_t byteCount = static_cast<uint8_t>(m_buffer.at(m_usesSerial ? 2 : 1));
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, byteCount + 4);
            }
            break;
        case FunctionCode::MASK_WRITE_REGISTER:
            extraction = m_buffer.substr(m_usesSerial ? 1 : 0, 9);
            break;
        case FunctionCode::READ_WRITE_REGISTERS:
            if (m_isServer)
            {
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, 5);
            }
            else
            {
                uint8_t byteCount = static_cast<uint8_t>(m_buffer.at(m_usesSerial ? 2 : 1));
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, byteCount * 2 + 4);
            }
            break;
        case FunctionCode::READ_FIFO:
            if (m_isServer)
            {
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, 5);
            }
            else
            {
                uint8_t byteCount = static_cast<uint8_t>(m_buffer.at(m_usesSerial ? 2 : 1));
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, byteCount * 2 + 7);
            }
            break;
        case FunctionCode::ENCAPSULATED:
            // currently not supported
            cout << "not supported" << endl;
            m_buffer = "";
            break;
        default:
            if (isError)
            {
                // when an exception is thrown, a function code, an exception code and the crc
                // are expected
                extraction = m_buffer.substr(m_usesSerial ? 1 : 0, 4);
            }
            else
            {
                throw std::invalid_argument("Received unknonwn function code!");
            }
        }
    }
    if (extraction.size() > 0)
    {
        m_buffer = m_buffer.substr(extraction.size() + (m_usesSerial ? 1 : 0));
    }
    if (m_usesSerial && id != m_ownId) extraction = "";
    return extraction;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagModbus::extractPDU()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

string PlagModbus::makeBytesFromDatagram(const std::shared_ptr<DatagramModbus> datagram) try
{
    string encodedBytes;
    //TODO: the actual encoding
    return encodedBytes;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagModbus::makeBytesFromDatagram()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}
