/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramUdp.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors: 
 * @brief Implementation of the DatagramUdp class
 * @version 0.1
 * @date 2022-11-13
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
#include "DatagramUdp.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Datagram:: Datagram object sets member values
 * 
 * @param sourcePlag the origin of this Datagram
 */
DatagramUdp::DatagramUdp(string sourcePlag) :
    Datagram(sourcePlag)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief creates a string representation of this Datagram
 * 
 * @return string 
 */
string DatagramUdp::toString() const
{
    string stringRepresentation = Datagram::toString();
    stringRepresentation += "{UDP info: sender: " + m_sender;
    stringRepresentation += "; receiver: " + m_receiver;
    stringRepresentation += "; port: " + to_string(m_port);
    stringRepresentation += "; payload: " + m_payload;
    stringRepresentation += "}";
    return stringRepresentation;
}
