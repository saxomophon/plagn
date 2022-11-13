/**
 *-------------------------------------------------------------------------------------------------
 * @file Datagram.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors: 
 * @brief Implementation of the Datagram class
 * @version 0.1
 * @date 2022-20-05
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
#include "Datagram.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Datagram:: Datagram object sets member values
 * 
 * @param sourcePlag the origin of this Datagram
 */
Datagram::Datagram() :
    m_sourceDatagramId(0),
    m_timeOfCreation(std::chrono::steady_clock::now())
{
    //TODO: create a unique hash of some sort
    m_ownId = 0;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief creates a string representation of this message
 * 
 * @return string 
 */
string Datagram::toString() const
{
    string stringRepresentation = "Datagram{";
    if (m_sourceDatagramId > 0) stringRepresentation += "generated from: "
                                                        + to_string(m_sourceDatagramId);
    if (m_sourceDatagramId == 0) "; newly generated";
    //TODO: string representation of time
    //stringRepresentation += ";at " + to_string(m_timeOfCreation);
    stringRepresentation += "; with id " + to_string(m_ownId);
    stringRepresentation += "}";
    return stringRepresentation;
}



/**
 * -------------------------------------------------------------------------------------------------
 * @brief Generates an outstream string representation for a Datagram
 * @param os is the stream this operator refers to
 * @param datagram the Datagram, that a string representation is wanted of
 *
 * @note Datagram::toString() is polymorphic (virtual) so the most specific implementation is used.
 */
ostream & operator<<(ostream & os, const std::shared_ptr<Datagram> datagram)
{
    return os << to_string(datagram);
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief Overload of to_string function of namespace std to include Datagram
 * @param datagram the Datagram, that a string representation is wanted of
 * @return result of stream operation
 *
 * @note Datagram::toString() is polymorphic (virtual) so the most specific implementation is used.
 */
string to_string(const std::shared_ptr<Datagram> datagram)
{
    return datagram ? datagram->toString() : "";
}
