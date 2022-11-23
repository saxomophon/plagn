/**
 *-------------------------------------------------------------------------------------------------
 * @file PropertyTreeReader.hpp
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

#ifndef PROPERTYTREEREADER_HPP_
#define PROPERTYTREEREADER_HPP_

// std incldues
#include <string>

// boost includes
#include <boost/property_tree/ptree.hpp>

class PropertyTreeReader
{
public:
    PropertyTreeReader(const boost::property_tree::ptree & propTree,
                       const std::string & rootName);

    // templacte functions ----

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief PropertyTreeReader::getParameter interfacing method to property_tree::ptree -get-
     * @param key name of a key at element of a rootName
     *
     * @return T (the parameter found at @p key of type T)
     */
    template <class T>
    T getParameter(const std::string & key) try
    {
        // ATTENTION! USING NAMESPACE BOOST::PROPERTY_TREE FOR BREVITY
        using namespace boost::property_tree;
        // the default of property tree is to use the dot as a separator. 
        // But for Plag'n config files, the "." is meant to allow nesting of some sort,
        // which *.ini files usually do not provide. Hence Plag'n goes  a little off-script
        // as far as *.ini-files go, the default boost intended to work, does not work for Plag'n
        return m_propertyTree.get<T>(ptree::path_type(m_rootName + "|" + key, '|'));
    }
    catch (std::exception & e)
    {
        throw std::runtime_error(std::string("Happened at PropertyTreeReader::getParameter: ") + e.what());
    }

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief PropertyTreeReader::getParameter static overload of getParameter, that does not require
     * an instance
     * @param propTree the boost::property_tree::ptree to perform the operation on
     * @param rootName the root name of the *.ini-file section (aka the name between [])
     * @param key name of a key at element of a @p rootName
     *
     * @return T (the parameter found at @p rootName | @p key of type T)
     */
    template <class T>
    static T getParameter(const boost::property_tree::ptree & propTree,
                          const std::string & rootName, const std::string & key) try
    {
        // ATTENTION! USING NAMESPACE BOOST::PROPERTY_TREE FOR BREVITY
        using namespace boost::property_tree;
        // as this might be used by Connection and some keys are split with a dot. This became necessary
        return propTree.get<T>(ptree::path_type(rootName + "|" + key, '|'));
    }
    catch (std::exception & e)
    {
        throw std::runtime_error(std::string("Happened at PropertyTreeReader::getParameter: ") + e.what());
    }

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief PropertyTreeReader::getOptionalParameter  interfacing method to property_tree::ptree
     * -get_optional-
     * @param key name of a key at element of a rootName
     * @param defaultValue the value to return, when there is no element under @p key
     */
    template <class T>
    T getOptionalParameter(const std::string & key, const T & defaultValue) try
    {
        boost::optional<T> tmp = m_propertyTree.get_optional<T>(m_rootName + "|" + key, '|');
        return (tmp.is_initialized()) ? tmp.value() : defaultValue;
    }
    catch (std::exception & e)
    {
        throw std::runtime_error(std::string("Happened at PropertyTreeReader::getOptionalParameter: ") + e.what());
    }

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief PropertyTreeReader::getOptionalParameter static overload of getOptionalParameter,
     * that does not require an instance
     * @param propTree the boost::property_tree::ptree to perform the operation on
     * @param rootName the root name of the *.ini-file section (aka the name between [])
     * @param key name of a key at element of a @p rootName
     * @param defaultValue the value to return, when there is no element under @p rootName | @p key
     *
     * @return T (the parameter found at @p rootName | @p key of type T)
     */
    template <class T>
    static T getOptionalParameter(const boost::property_tree::ptree & propTree,
                                  const std::string & rootName, const std::string & key,
                                  const T & defaultValue) try
    {
        // ATTENTION! USING NAMESPACE BOOST::PROPERTY_TREE FOR BREVITY
        using namespace boost::property_tree;
        boost::optional<T> tmp = propTree.get_optional<T>(rootName + "|" + key, '|');
        return (tmp.is_initialized()) ? tmp.value() : defaultValue;
    }
    catch (std::exception & e)
    {
        throw std::runtime_error(std::string("Happened at PropertyTreeReader::getOptionalParameter: ") + e.what());
    }

    // end of template functions ---

protected:
    const boost::property_tree::ptree & m_propertyTree; //!< content of the config file

private:
    std::string m_rootName; //<! the section-name (aka name in []) of this
};

#endif //PROPERTYTREEREADER_HPP_