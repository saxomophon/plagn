/**
 *-------------------------------------------------------------------------------------------------
 * @file Utilities.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Utility functions and general utilities collection
 * @version 0.1
 * @date 2022-11-11
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

#ifndef UTILITIES_HPP
#define UTILITIES_HPP

// std includes
#include <string>

namespace plagn
{
/**
 * -------------------------------------------------------------------------------------------------
 * @brief The default path where plagn will search for a configuration file, if no explicit path
 * is specified
 * @note This path is OS dependant.
 */
    extern const std::string DEFAULT_CONFIG_PATH;
}

std::string getTimeAsUtcIsoStr(const std::time_t & timePoint);



#endif // UTILITIES_HPP