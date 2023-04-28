/**
 * -------------------------------------------------------------------------------------------------
 * @file TransportLayer.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements some of the TransportLayer interface
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

// self include
#include "TransportLayer.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Transport Layer:: Transport Layer object by setting default values
 * 
 * @param timeout reference time in ms after which to timeout when e.g. receiving bytes
 */
TransportLayer::TransportLayer(const std::chrono::milliseconds & timeout) :
    m_type(undefined_type),
    m_timeout(timeout)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Destroy the Transport Layer:: Transport Layer object
 * @details empty stub, that was needed due to compiler errors
 *
 */
TransportLayer::~TransportLayer()
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 * 
 * @return LayerType member value
 */
TransportLayer::LayerType TransportLayer::getType() const
{
    return m_type;
}