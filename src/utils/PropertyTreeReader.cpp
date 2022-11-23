/**
 *-------------------------------------------------------------------------------------------------
 * @file PropertyTreeReader.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Kind of a convenience interface for boost::ptree
 * @version 0.1
 * @date 2022-05-20
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
#include "PropertyTreeReader.hpp"

PropertyTreeReader::PropertyTreeReader(const boost::property_tree::ptree & propTree,
                                       const std::string & rootName) :
    m_propertyTree(propTree),
    m_rootName(rootName)
{
}