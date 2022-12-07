/**
 * -------------------------------------------------------------------------------------------------
 * @file MqttInterface.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements some of the MqttInterfaceInterface abstract class
 * @version 0.1
 * @date 2022-12-06
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
#include <iostream>

// self include
#include "MqttInterface.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Mqtt Interface:: Mqtt Interface object inits member values
 * 
 * @param parent the Plag this is a child item of
 */
MqttInterface::MqttInterface(const Plag & parent, const string & brokerIP, unsigned int brokerPort) :
    m_parent(parent),
    m_brokerIP(brokerIP),
    m_brokerPort(brokerPort)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief polls data and checks existence of messages in buffer
 * 
 * @return true if buffer has a Datagram
 * @return false else
 * @sa MqttInterface::poll()
 */
bool MqttInterface::hasMessages()
{
    poll();
    return m_incomingBuffer.begin() != m_incomingBuffer.end();
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief get Datagram from parsed buffer
 * 
 * @return shared_ptr<DatagramMqtt> 
 */
shared_ptr<DatagramMqtt> MqttInterface::getMessage()
{
    if (m_incomingBuffer.begin() == m_incomingBuffer.end()) return nullptr;
    shared_ptr<DatagramMqtt> extractedDatagram = m_incomingBuffer.front();
    m_incomingBuffer.pop_front();
    return extractedDatagram;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief turns a regular binary string into an MQTT string
 * @details see for definition documentation chapter 1.5.3 (MQTT v.3.1.1 doc) or 1.5.4 (MQTT v.5 doc)
 * @param text the raw string
 * @return string two bytes of length indication and then @p text
 */
string MqttInterface::makeMqttString(const string & text) const try
{
    uint16_t size = static_cast<uint16_t>(std::min(static_cast<size_t>(UINT16_MAX), text.size()));
    string mqttText;
    mqttText += static_cast<char>(static_cast<uint8_t>((size & 0xFF00) >> 8) & 0xFF);
    mqttText += static_cast<char>(static_cast<uint8_t>(size & 0xFF));
    mqttText += text.substr(0, size);
    return mqttText;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttInterface::makeMqttString()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief reads an MQTT string from an arbitrarily long string and returns just the content
 * @details the name extract indicates, that the @p text given will be shortened by the entire
 *          MQTT string, that got read
 *          see for definition documentation chapter 1.5.3 (MQTT v.3.1.1 doc) or 1.5.4 (MQTT v.5 doc)
 * @param text the raw data received from an MQTT broker (may be longer than needed). WILL BE shortened
 * @param startPos where to start the extraction (default = 0)
 * @return string the content of the MQTT string (or everything after the first two bytes,
 *         should length not match)
 */
string MqttInterface::extractMqttString(string & text, size_t startPos) const try
{
    uint16_t size = static_cast<uint8_t>(text.at(startPos + 1));
    size |= static_cast<uint16_t>(text.at(startPos)) << 8;
    string content = "";
    if (text.size() - 2 - startPos < size)
    {
        cout << "Received MQTT string, that is smaller than indicated: " << text << endl;
        content = text.substr(2 + startPos);
        text = "";
    }
    else
    {
        content = text.substr(2 + startPos, size);
        text = string((startPos > 0 ? text.substr(0, startPos) : ""))
            + string((text.size() > size + 2 + startPos) ? text.substr(size + 2 + startPos) : "");
    }
    return content;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttInterface::extractMqttString()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief generates the fixed header of an MQTT telegram and prepends it to @p content .
 * @details An MQTT fixed header always has two item, the first containing @p type and @p flags in
 *          one byte, while the second item (in as few bytes as possible) indicates the length of
 *          @p content .
 * @param type type of the MQTT telegram
 * @param flags the flags for this telegram (dependent on @p type )
 * @param content the actual content the fixed header is for
 */
void MqttInterface::prepareFixedHeader(MqttMessageType type, uint8_t flags, string & content) const try
{
    string header = "";
    char firstByte = flags & 0x0F;
    firstByte |= (type & 0x0F) << 4;

    header += firstByte;

    // we are allowd a maxium of 28 bit of length
    uint32_t length = content.size() & 0x0FFFFFFF;

    // starting with LSB append current "byte"
    header += static_cast<char>(length & 0x7F);
    while (length > 0x7F)
    {
        // mask the first bit of the last byte, as we know, that there is data following
        header.back() |= 0x80;
        // we already wrote 7 bit (either before or last loop), so we'll jump ahead
        length >>= 7;
        // append the current byte
        header += static_cast<char>(length & 0x7F);
    }
    // prepend the first byte (which means, that we are done)
    content.insert(0, header);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttInterface::parseIncomingBuffer()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief evaluates buffer for complete MQTT telegrams and calls appropriate parsers.
 * @details this not only evaluates @p inBuffer , but also shortens it by the evaluated part
 *
 * @param inBuffer the buffer to evaluate and shorten (if appropriate)
 * @return 0, if buffer contained a complete message
 * @return number of bytes until message is complete, else
 */
size_t MqttInterface::parseIncomingBuffer(string & inBuffer) try
{
    // data too litte to contain anything
    if (inBuffer.size() < 2) return 0;
    while (inBuffer.size() > 2)
    {
        size_t currentBytePos = 0;
        uint8_t firstHalfByte = (inBuffer.at(currentBytePos) & 0xF0) >> 4;
        MqttMessageType msgType = static_cast<MqttMessageType>(firstHalfByte);
        uint8_t flags = inBuffer.at(currentBytePos) & 0x0F;
        size_t packetLength = inBuffer.at(++currentBytePos) & 0x7F;
        while ((inBuffer.at(currentBytePos) & 0x80) == 0x80
               && inBuffer.size() > currentBytePos + 1
               && currentBytePos < 5)
        {
            currentBytePos++;
            packetLength |= static_cast<size_t>(inBuffer.at(currentBytePos) & 0x7F)
                            << (currentBytePos - 1) * 7;
        }
        // data does not contain entire package -> wait
        if (inBuffer.size() <= currentBytePos + packetLength)
        {
            return currentBytePos + packetLength - inBuffer.size() + 1;
        }
        uint8_t packetType = (inBuffer.at(0) & 0xF0) >> 4;
        switch (packetType)
        {
        case CONNECT:
            {
                string connect = inBuffer.substr(2, packetLength + 2);
                parseConnect(connect);
            }
            break;
        case CONNACK:
            {
                string connack = inBuffer.substr(0, packetLength + 2);
                parseConnAck(connack);
            }
            break;
        case PUBLISH:
            {
                string publish = inBuffer.substr(2, packetLength + 2);
                uint8_t firstByte = inBuffer.at(0);
                parsePublish(firstByte, publish);
            }
            break;
        case PUBACK:
            {
                string puback = inBuffer.substr(0, packetLength + 2);
                parsePubAck(puback);
            }
            break;
        case PUBREC:
            {
                string pubRec = inBuffer.substr(0, packetLength + 2);
                parsePubRec(pubRec);
            }
            break;
        case PUBREL:
            {
                string pubRel = inBuffer.substr(0, packetLength + 2);
                parsePubRel(pubRel);
            }
            break;
        case PUBCOMP:
            {
                string pubComplete = inBuffer.substr(0, packetLength + 2);
                parsePubComp(pubComplete);
            }
            break;
        case SUBACK:
            {
                string suback = inBuffer.substr(0, packetLength + 2);
                parseSubAck(suback);
            }
            break;
        case UNSUBACK:
            {
                string unsuback = inBuffer.substr(0, packetLength + 2);
                parseUnsubAck(unsuback);
            }
            break;
        case AUTH:
            {
                string auth = inBuffer.substr(2, packetLength + 2);
                parseAuth(auth);
            }
            break;
        case PINGREP:
            if (packetLength != 0) cout << "Unexpected packet length for PINGREP" << endl;
            break;
        default:
            cout << "Unknown packet type: " + to_string(packetType) << endl;
        }
        m_lastTimeReceived = std::chrono::steady_clock::now();
        inBuffer = (inBuffer.size() > packetLength + 2) ? inBuffer.substr(packetLength + 2) : "";
    }
    return 0;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttInterface::parseIncomingBuffer()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief convenience function to generate correct packet identifier
 * @details an identifier is an unsigned 16 bit integer. it may not be a duplicate of an identifier
 * that is assigned to a currently handled packet. This function hence counts the identifier up
 * and avoids the 0, as it is not allowed, and any identifiers, that still might be in the loop
 * @return uint16_t a value between 1 and 65535, that is currently free
 */
uint16_t MqttInterface::generateIdentifier() try
{
    do
    {
        ++m_currentIdentifier;
    } while (m_nonAckedData.count(m_currentIdentifier) > 0 || m_currentIdentifier == 0);

    return m_currentIdentifier;
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttInterface::generateIdentifier()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief convenience function to keep a sent packet in the loop until it is resolved
 * @details some packets (definitely NOT all) require acknowledgements and should be resent, if
 * that acknowledgement fails to come. Therefore we'll hold that packet with current time information
 * in a map, tied to its identifier, for quick access. This function adds the packet to the map.
 * @param identifier packet identifier the broker knows this packet by
 * @param data the raw binary data, that was handed to TcpClient, as to avoid regenerating it
 * @sa MqttInterface::generateIdentifier()
 * @sa MqttInterface::removeNonAcknowledgedData()
 * @sa MqttInterface::resendOldData()
 */
void MqttInterface::addNonAcknowledgedData(uint16_t identifier, const string & data) try
{
    //ATTENTION: using namespace for brevity
    using namespace std::chrono;
    std::pair<string, steady_clock::time_point> value(data, steady_clock::now());
    m_nonAckedData.insert_or_assign(identifier, value);
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttInterface::addNonAcknowledgedData()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief convenience function to remove a packet from the loop, assuming it was resolved
 * @details some packets (those with QoS > 0) require acknowledgements and should be resent, if
 * that acknowledgement fails to come. Therefore we'll hold that packet with current time information
 * in a map, tied to its identifier, for quick access. This function removes the packet from the map.
 * @param identifier packet identifier the broker knows this packet by
 * @sa MqttInterface::addNonAcknowledgedData()
 */
void MqttInterface::removeNonAcknowledgedData(uint16_t identifier) try
{
    if (m_nonAckedData.count(identifier) > 0)
    {
        m_nonAckedData.erase(identifier);
    }
}
catch (exception & e)
{
    string errorMsg = e.what();
    errorMsg += "\nSomething happened in MqttInterface::removeNonAcknowledgedData()";
    runtime_error eEdited(errorMsg);
    throw eEdited;
}