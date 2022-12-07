/**
 *-------------------------------------------------------------------------------------------------
 * @file TransportLayer.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the TransportLayer interface
 * @version 0.1
 * @date 2022-11-26
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

#ifndef TRANSPORTLAYER_HPP_
#define TRANSPORTLAYER_HPP_

// std includes
#include <chrono>
#include <string>

/**
 *-------------------------------------------------------------------------------------------------
 * @brief This is an interface for any kind of non-application, non-physical level layer protocols.
 * @details The OSI layer model is imperfect, in that it cannot describe the communication world
 * in its details and reality correctly (see e.g. https://jvns.ca/blog/2021/05/11/what-s-the-osi-model-/).
 * However, taking the idea of the application level and transport Level being seperate, we would
 * consider Plags to only translate the application level by design. Hence, the transport layers
 * should be interchangable. MQTT - at least in theory - ***should*** work with a serial connection.
 * So to provide Plags, that actually allow interchangable transport level protocols, we provide this
 * interface and its derivative implementation, so that the Plags do not need to implement them
 * individually.
 */
class TransportLayer
{
public:
    // types ------------------
    /**
     * ---------------------------------------------------------------------------------------------
     * @brief enum for descriptive identification of TransportLayer types
     * 
     */
    enum LayerType
    {
        TCP_CLIENT,
        TLS_CLIENT,
        TCP_SERVER_ONE_CLIENT,
        TLS_SERVER_ONE_CLIENT,
        SERIAL,
        undefined_type
    };
    // methods ----------------
    TransportLayer(const std::chrono::milliseconds & timeout);

    virtual LayerType getType() const;

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief Get the currently available number of bytes by checking
     *
     * @return size_t number of bytes
     */
    virtual size_t getAvailableBytesCount() = 0;

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief gets the bytes from the transport level and hands them to you
     * 
     * @param numberOfBytes number of bytes you want to receive
     * @return std::string @p numberOfBytes raw bytes as a string, or all available bytes when
     * @p numberOfBytes was 0.
     * @throws std::runtime_error 
     */
    virtual std::string receiveBytes(size_t numberOfBytes = 0) = 0;

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief gets an exact number of bytes or nothing from transport level to you
     * 
     * @param numberOfBytes number of bytes you want to receive
     * @return std::string  @p numberOfBytes raw bytes as a string, or emoty string
     */
    virtual std::string peekAndReceive(size_t numberOfBytes) = 0;

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief sends the @p appData via the implemented transport level to somewhere
     *
     * @param appData the application level encoded data to send
     */
    virtual void transmit(const std::string & appData) = 0;

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief connects the transport level to its counter point
     * 
     * @param timeout specific timeout for connection, will use defaultm when 0
     */
    virtual void connect(const std::chrono::milliseconds & timeout) = 0;

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief disconnects the transport level from its counter point
     * 
     */
    virtual void disconnect() = 0;

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief connection status indication by actually checking on it
     * 
     * @return true if this seems connected
     * @return false otherwise
     */
    virtual bool isConnected() = 0;

protected:
    LayerType m_type;                    //!< type of layer
    std::chrono::milliseconds m_timeout; //!< reference time to timeout operations
};

#endif /*TRANSPORTLAYER_HPP_*/