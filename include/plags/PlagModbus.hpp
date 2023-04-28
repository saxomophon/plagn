/**
 *-------------------------------------------------------------------------------------------------
 * @file PlagModbus.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the PlagModbus class
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

#ifndef PLAGMODBUS_HPP
#define PLAGMODBUS_HPP

// std includes

// own includes
#include "DatagramModbus.hpp"
#include "Plag.hpp"
#include "TcpClient.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The PlagMqtt class is a Plag to interact with the MQTT protocol
 *
 */
class PlagModbus: public Plag
{
public:
    enum FunctionCode : uint8_t
    {
        READ_COIL                = 0x01, //!< see Modbus Protocol Application Specification V1.1b3 p.11
        READ_INPUT               = 0x02, //!< see Modbus Protocol Application Specification V1.1b3 p.12
        READ_HOLDING_REGISTER    = 0x03, //!< see Modbus Protocol Application Specification V1.1b3 p.15
        READ_INPUT_REGISTER      = 0x04, //!< see Modbus Protocol Application Specification V1.1b3 p.16
        WRITE_SINGLE_COIL        = 0x05, //!< see Modbus Protocol Application Specification V1.1b3 p.17
        WRITE_SINGLE_REGISTER    = 0x06, //!< see Modbus Protocol Application Specification V1.1b3 p.19
        READ_EXCEPTION           = 0x07, //!< see Modbus Protocol Application Specification V1.1b3 p.20
        DIAGNOSTICS              = 0x08, //!< see Modbus Protocol Application Specification V1.1b3 p.21
        GET_COM_COUNTER          = 0x0B, //!< see Modbus Protocol Application Specification V1.1b3 p.25
        GET_COM_LOG              = 0x0C, //!< see Modbus Protocol Application Specification V1.1b3 p.26
        ENCAPSULATED_CANOPEN     = 0x0D, //!< see Modbus Protocol Application Specification V1.1b3 p.42
        ENCAPSULATED_DEVICE_INFO = 0x0E, //!< see Modbus Protocol Application Specification V1.1b3 p.43
        WRITE_COILS              = 0x0F, //!< see Modbus Protocol Application Specification V1.1b3 p.29
        WRITE_REGISTERS          = 0x10, //!< see Modbus Protocol Application Specification V1.1b3 p.30
        REPORT_SERVER_ID         = 0x11, //!< see Modbus Protocol Application Specification V1.1b3 p.31
        READ_FILE                = 0x14, //!< see Modbus Protocol Application Specification V1.1b3 p.32
        WRITE_FILE               = 0x15, //!< see Modbus Protocol Application Specification V1.1b3 p.34
        MASK_WRITE_REGISTER      = 0x16, //!< see Modbus Protocol Application Specification V1.1b3 p.36
        READ_WRITE_REGISTERS     = 0x17, //!< see Modbus Protocol Application Specification V1.1b3 p.38
        READ_FIFO                = 0x18, //!< see Modbus Protocol Application Specification V1.1b3 p.40
        ENCAPSULATED             = 0x2B, //!< see Modbus Protocol Application Specification V1.1b3 p.41
        EXCEPTION_MASK           = 0x80  //!< see Modbus Protocol Application Specification V1.1b3 p.47
    };

    enum ExceptionCode : uint8_t //!< see Modbus Protocol Application Specification V1.1b3 p.48-49
    {
        ILLEGAL_FUNCTION         = 0x01,
        ILLEGAL_DATA_ADDRESS     = 0x02,
        ILLEGAL_DATA_VALUE       = 0x03,
        SERVER_FAILURE           = 0x04,
        ACKNOWLEDGE              = 0x05,
        SERVER_BUSY              = 0x06,
        MEMORY_ERROR             = 0x08,
        GATEWAY_UNAVAILABLE      = 0x0A,
        NO_RESPONSE_FROM_GATEWAY = 0x0B
    };
public:
    PlagModbus(const boost::property_tree::ptree & propTree,
               const std::string & name, const uint64_t & id);
    ~PlagModbus();

    virtual void readConfig();

    virtual void init();

    virtual bool loopWork();

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram);

private:
    void parseBuffer();
    std::string extractPDU();
    std::string makeBytesFromDatagram(const std::shared_ptr<DatagramModbus> datagram);

private:
    // config parameters
    bool m_isServer;                //!< whether this acts as server (in old docs referred to as master) or client (=> false)
    bool m_usesSerial;              //!< whether this is connected overs serial (=>true) or TCP (=>false)
    int16_t m_ownId;                //!< the own id in serial connection
    std::string m_connectionType;   //!< type of connetion (e.g. Serial, TCP Client, TCP Server, ...)
    std::string m_comPort;          //!< port for serial connection
    uint32_t m_baudRate;            //!< baud rate for serial conenction
    std::string m_parity;           //!< parity for serial
    uint8_t m_byteSize;             //!< size of a "byte"
    uint8_t m_stopBits;             //!< stop bit count times ten for serial connection
    std::string m_serverIP;         //!< IP for TCP connection as client
    uint16_t m_tcpPort;             //!< port number for TCP connection (any)

    // worker members
    std::string m_buffer;                        //!< where to read received data to
    std::unique_ptr<TransportLayer> m_dataLayer; //!< access to data stream
};

#endif // PLAGMODBUS_HPP
