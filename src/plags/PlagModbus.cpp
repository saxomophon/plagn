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
        uint16_t registerCount = 0;
        uint16_t startRegister = 0xFFFF;
        switch (functionCode)
        {
        case FunctionCode::READ_COIL:
        case FunctionCode::READ_INPUT: // deliberate fall-through
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), functionCode,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::READ_HOLDING_REGISTER:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                uint16_t value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::READ_HOLDING_REGISTER,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::READ_INPUT_REGISTER:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::READ_INPUT_REGISTER,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::WRITE_SINGLE_COIL:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::WRITE_SINGLE_COIL,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::WRITE_SINGLE_REGISTER:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::WRITE_SINGLE_REGISTER,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::READ_EXCEPTION:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::READ_EXCEPTION,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::DIAGNOSTICS:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::DIAGNOSTICS,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::GET_COM_COUNTER:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::GET_COM_COUNTER,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::GET_COM_LOG:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::GET_COM_LOG,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::WRITE_COILS:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::WRITE_COILS,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::WRITE_REGISTERS:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::WRITE_REGISTERS,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::REPORT_SERVER_ID:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::REPORT_SERVER_ID,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::READ_FILE:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::READ_FILE,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::WRITE_FILE:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::WRITE_FILE,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::MASK_WRITE_REGISTER:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::MASK_WRITE_REGISTER,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::READ_WRITE_REGISTERS:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::READ_WRITE_REGISTERS,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
            break;
        case FunctionCode::READ_FIFO:
            registerCount = static_cast<uint16_t>(pdu[1]) << 8;
            registerCount |= static_cast<uint8_t>(pdu[2]);
            startRegister = static_cast<uint16_t>(pdu[3]) << 8;
            startRegister |= static_cast<uint8_t>(pdu[4]);
            for (size_t i = 0; i < registerCount; i++)
            {
                shared_ptr<DatagramModbus> datagram;
                bool value = extractCoilFromPDU(i, pdu);
                datagram = make_shared<DatagramModbus>(this->getName(), FunctionCode::READ_FIFO,
                                                       value, startRegister + i);
                m_outgoingDatagrams.push_back(datagram);
            }
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

bool PlagModbus::extractCoilFromPDU(uint16_t pos, const std::string & pdu) try
{
    uint16_t offset = 5;
    uint16_t bytePos = pos / 8;
    uint8_t bitPos = pos % 8;
    uint8_t bitMask = 1 << (15 - bitPos);
    bool result = pdu[offset + bytePos] & bitMask;
    return result;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagModbus::extractCoilFromPDU()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Extract a word (e.g. a register'S result) from a PDU
 * @details This is a convenience function. As it might be possible (though it is discouraged), that
 * a read result, will have more than one register read.
 * @param pos Word position in the result (e.g. first word, second word). Starting at 0.
 * @param reg Register adress, that is currently read.
 * @param pdu The entire PDU (aka process data unit)
 * @return DataType As the actual type is not specified, we'll use this convenience type.
 * @remark It is highly discouraged to read among differently sized types with one request, as if
 * will cause errors
 */
DataType PlagModbus::extractWordFromPDU(uint16_t pos, uint16_t reg, const std::string & pdu) try
{
    uint16_t offset = 5;
    uint8_t byteCount = 2;
    ModbusDataType modbusType = ModbusDataType::UINT16;
    if (m_registerToType.count(reg)) modbusType = m_registerToType[reg];
    if (modbusType == ModbusDataType::INT32
        || modbusType == ModbusDataType::UINT32
        || modbusType == ModbusDataType::FLOAT32
        || modbusType == ModbusDataType::FLOAT64)
    {
        byteCount = (modbusType == ModbusDataType::FLOAT64) ? 8 : 4;
    }
    // NOTE: it is highly discouraged to read among differently sized types with one request
    uint16_t byteStartPos = pos * byteCount;
    vector<uint8_t> bytes(byteCount, 0);
    for (size_t i = 0; i < byteCount; i++)
    {
        uint8_t wordPos = i / 2;
        if (m_wordsAreSwapped) wordPos = byteCount - wordPos;
        uint8_t bytePos = (i % 2 == 0) ? 0 : 1;
        if (m_bytesAreSwapped) bytePos = (bytePos == 0) ? 1 : 0;
        bytes[wordPos * 2 + bytePos] = pdu[byteStartPos + i];
    }
    DataType result;
    switch (modbusType)
    {
    case ModbusDataType::INT16:
        {
            int16_t preResult = bytes.at(0) << 8;
            preResult |= bytes.at(1);
            result = DataType(preResult);
        }
        break;
    case ModbusDataType::UINT16:
        {
            uint16_t preResult = bytes.at(0) << 8;
            preResult |= bytes.at(1);
            result = DataType(preResult);
        }
        break;
    case ModbusDataType::INT32:
        {
            int32_t preResult = bytes.at(0) << 24;
            preResult |= bytes.at(1) << 16;
            preResult |= bytes.at(2) << 8;
            preResult |= bytes.at(3);
            result = DataType(preResult);
        }
        break;
    case ModbusDataType::UINT32:
        {
            uint32_t preResult = bytes.at(0) << 24;
            preResult |= bytes.at(1) << 16;
            preResult |= bytes.at(2) << 8;
            preResult |= bytes.at(3);
            result = DataType(preResult);
        }
        break;
    case ModbusDataType::FLOAT16:
        {
            //TODO: translate
        }
        break;
    case ModbusDataType::FLOAT32:
        {
            float preResult = 0.f;
            unsigned char * charCast = reinterpret_cast<unsigned char *>(&preResult);
            for (size_t i = 0; i < byteCount; i++)
            {
                charCast[i] = bytes.at(i);
            }
            result = DataType(preResult);
        }
        break;
    case ModbusDataType::FLOAT64:
        {
            double preResult = 0.;
            unsigned char * charCast = reinterpret_cast<unsigned char *>(&preResult);
            for (size_t i = 0; i < byteCount; i++)
            {
                charCast[i] = bytes.at(i);
            }
            result = DataType(preResult);
        }
        break;
    }
    return result;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in PlagModbus::extractWordFromPDU()";
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
