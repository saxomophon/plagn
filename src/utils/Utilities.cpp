/**
 *-------------------------------------------------------------------------------------------------
 * @file Utilities.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implementation of utility functions
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

// self include
#include "Utilities.hpp"

using namespace std;

#ifdef _WIN32
const string plagn::DEFAULT_CONFIG_PATH = "./plagn.conf";
#else
const string plagn::DEFAULT_CONFIG_PATH = "/usr/etc/plagn.conf";
#endif


/**
 * -------------------------------------------------------------------------------------------------
 * @brief generates a string representation of time in the ISO-format "YYYY-mm-DDThh:mm:ssZ"
 * @param timePoint seconds since epoch (epoch starts at 1970-01-01 00:00:00 UTC)
 * @remark The hard-coded 21 is a result of the format
 */
string getTimeAsUtcIsoStr(const std::time_t & timePoint)
{
    struct tm * timeinfo;
    char buffer[21];
    string timeAsStr = "1970-01-01T00:00:00Z";

    timeinfo = std::gmtime(&timePoint);

    strftime(buffer, 21, "%FT%TZ", timeinfo);
    for (size_t i = 0; i < 21 && i < timeAsStr.size(); i++)
    {
        timeAsStr[i] = buffer[i];
    }
    return timeAsStr;
}