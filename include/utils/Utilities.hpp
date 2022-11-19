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
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace plagn
{
    /**
     * ---------------------------------------------------------------------------------------------
     * @brief The default path where plagn will search for a configuration file, if no explicit path
     * is specified
     * @note This path is OS dependant.
     */
    extern const std::string DEFAULT_CONFIG_PATH;

    /**
     * ---------------------------------------------------------------------------------------------
     * @brief makes switch-case-operation possbile for Datatype conversion
     * 
     */
    enum DataTypeIndex : uint8_t
    {
        INT    = 0,
        UINT   = 1,
        INT64  = 2,
        UINT64 = 3,
        DOUBLE = 4,
        STRING = 5,
        MAP    = 6,
        VECTOR = 7
    };
}

// typedef
/**
 *-------------------------------------------------------------------------------------------------
 * @brief Typedef for a long std::variant, that is at the core of Kable's conversion
 * @remark Typedefs are annoying. At least to me. But with the std::variant being at the core
 * of the conversion a Kable needs to perform, and its declaration being so ridiculously long, this
 * is the **one** exception (***ONE!***) I'll tolerate. Otherwise the style-guides rule to avoid
 * typedefs holds true!
 */
typedef std::variant<int, unsigned int, int64_t, uint64_t, double,
                     std::string, std::map<std::string, std::string>,
                     std::vector<std::string>> DataType;

int convertDataTypeToInt(const DataType & value, int defaultValue = 0);
unsigned int convertDataTypeToUint(const DataType & value, unsigned int defaultValue = 0);
int64_t convertDataTypeToInt64(const DataType & value, int64_t defaultValue = 0);
uint64_t convertDataTypeToUint64(const DataType & value, uint64_t defaultValue = 0);
double convertDataTypeToDouble(const DataType & value, double defaultValue = 0.);
std::string convertDataTypeToString(const DataType & value);
std::map<std::string, std::string> convertDataTypeToMap(const DataType & value);
std::vector<std::string> convertDataTypeToVector(const DataType & value);

bool isDigit(char c) noexcept;
bool startsWith(char c, const std::string & text);
bool endsWith(char c, const std::string & text);


std::string getTimeAsUtcIsoStr(const std::time_t & timePoint);


std::ostream & operator<<(std::ostream & os, const std::vector<std::string> & strVector);
std::ostream & operator<<(std::ostream & os, const std::map<std::string, std::string> & strMap);

std::string to_string(const std::vector<std::string> & strVector);
std::string to_string(const std::map<std::string, std::string> & strMap);



#endif // UTILITIES_HPP