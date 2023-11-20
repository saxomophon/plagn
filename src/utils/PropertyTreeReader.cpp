/**
 *-------------------------------------------------------------------------------------------------
 * @file PropertyTreeReader.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Kind of a convenience interface for boost::ptree
 * @version 0.1
 * @date 2022-11-23
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
#include <vector>

// self include
#include "PropertyTreeReader.hpp"

using namespace std;

PropertyTreeReader::PropertyTreeReader(const boost::property_tree::ptree & propTree,
                                       const std::string & rootName) :
    m_propertyTree(propTree),
    m_rootName(rootName)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief collects the list of keys, found under the root item
 * 
 * @return vector<string> 
 */
vector<string> PropertyTreeReader::getKeys() try
{
    vector<string> keys;
    const boost::property_tree::ptree & childTree = m_propertyTree.get_child(m_rootName);
    for (const std::pair<std::string, boost::property_tree::ptree> & item : childTree)
    {
        keys.push_back(item.first);
    }
    return keys;
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in PropertyTree::getKeys(): ") + e.what());
}