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

// std includes
#include <sstream>

// self include
#include "Utilities.hpp"

using namespace std;

#ifdef _WIN32
const string plagn::DEFAULT_CONFIG_PATH = "./plagn.conf";
#else
const string plagn::DEFAULT_CONFIG_PATH = "/usr/etc/plagn.conf";
#endif

/**
 *-------------------------------------------------------------------------------------------------
 * @brief extracts or converts a value of unspecified DataType into an integer
 * 
 * @param value instane of a std::variant (=typedef'ed to DataType)
 * @param defaultValue what this value will equate to, for some errors
 * @return int
 * @remark When to throw an exception and when to use @p defaultValue has no clear rules, yet
 */
int convertDataTypeToInt(const DataType & value, int defaultValue) try
{
    int returnValue = defaultValue;
    switch (value.index())
    {
    case plagn::INT:
        return get<int>(value);
    case plagn::UINT:
        if (get<unsigned int>(value) > INT_MAX) return defaultValue;
        return static_cast<int>(get<unsigned int>(value));
    case plagn::INT64:
        {
            int64_t val = get<int64_t>(value);
            if (val > INT_MAX || val < INT_MIN) return defaultValue;
            return static_cast<int>(val);
        }
    case plagn::UINT64:
        if (get<uint64_t>(value) > INT_MAX) return defaultValue;
        return static_cast<int>(get<uint64_t>(value));
    case plagn::DOUBLE:
        {
            double val = get<double>(value);
            if (val > INT_MAX || val < INT_MIN) return defaultValue;
            return static_cast<int>(val);
        }
    case plagn::STRING:
        {
            string valueAsStr = get<string>(value);
            if (valueAsStr == ""
                || (!isDigit(valueAsStr.front())
                    && static_cast<int>(valueAsStr.front()) != 0x2D)
                || (static_cast<int>(valueAsStr.front()) == 0x2D
                    && (valueAsStr.size() < 2
                        || !isDigit(valueAsStr[1]))))
            {
                throw std::invalid_argument("Expected number. Got string!");
            }
            return stoi(valueAsStr, nullptr, 0);
        }
    case plagn::MAP:
        {
            map<string, string> valueAsMap = get<map<string, string>>(value);
            if (valueAsMap.count("value") > 0) return convertDataTypeToInt(valueAsMap["value"]);
            throw std::invalid_argument("Did not find map entry for conversion to Int!");
        }
    case plagn::VECTOR:
        {
            vector<string> valueAsVector = get<vector<string>>(value);
            if (valueAsVector.size() > 0) return convertDataTypeToInt(valueAsVector.front());
            throw std::invalid_argument("Cannot use empty vector for conversion to Int!");
        }
    }
    return returnValue;
}
catch (exception & e)
{
    throw std::runtime_error("happened here");
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief extracts or converts a value of unspecified DataType into an usigned integer
 *
 * @param value instane of a std::variant (=typedef'ed to DataType)
 * @param defaultValue what this value will equate to, for some errors
 * @return unsigned int
 * @remark When to throw an exception and when to use @p defaultValue has no clear rules, yet
 */
unsigned int convertDataTypeToUint(const DataType & value, unsigned int defaultValue) try
{
    unsigned int returnValue = defaultValue;
    switch (value.index())
    {
    case plagn::INT:
        {
            int valAsInt = get<int>(value);
            if (valAsInt < 0) throw std::invalid_argument("Can't convert numbers < 0 to unsigned!");
            return static_cast<unsigned int>(valAsInt);
        }
    case plagn::UINT:
        return get<unsigned int>(value);
    case plagn::INT64:
        {
            int64_t val = get<int64_t>(value);
            if (val < 0) throw std::invalid_argument("Can't convert numbers < 0 to unsigned!");
            if (val > UINT_MAX) return defaultValue;
            return static_cast<unsigned int>(val);
        }
    case plagn::UINT64:
        if (get<uint64_t>(value) > UINT_MAX) return defaultValue;
        return static_cast<unsigned int>(get<uint64_t>(value));
    case plagn::DOUBLE:
        {
            double val = get<double>(value);
            if (val < 0) throw std::invalid_argument("Can't convert numbers < 0 to unsigned!");
            if (val > UINT_MAX) return defaultValue;
            return static_cast<unsigned int>(val);
        }
    case plagn::STRING:
        {
            string valueAsStr = get<string>(value);
            if (valueAsStr == ""
                || (!isDigit(valueAsStr.front())
                    && static_cast<int>(valueAsStr.front()) != 0x2D))
            {
                throw std::invalid_argument("Expected unsigned number. Got string!");
            }
            return convertDataTypeToUint(stoul(valueAsStr, nullptr, 0));
        }
    case plagn::MAP:
        {
            map<string, string> valueAsMap = get<map<string, string>>(value);
            if (valueAsMap.count("value") > 0) return convertDataTypeToUint(valueAsMap["value"]);
            throw std::invalid_argument("Did not find map entry for conversion to Uint!");
        }
    case plagn::VECTOR:
        {
            vector<string> valueAsVector = get<vector<string>>(value);
            if (valueAsVector.size() > 0) return convertDataTypeToUint(valueAsVector.front());
            throw std::invalid_argument("Cannot use empty vector for conversion to Uint!");
        }
    }
    return returnValue;
}
catch (exception & e)
{
    throw std::runtime_error("happened here");
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief extracts or converts a value of unspecified DataType into a 64-bit integer
 *
 * @param value instane of a std::variant (=typedef'ed to DataType)
 * @param defaultValue what this value will equate to, for some errors
 * @return int64_t
 * @remark When to throw an exception and when to use @p defaultValue has no clear rules, yet
 */
int64_t convertDataTypeToInt64(const DataType & value, int64_t defaultValue) try
{
    int64_t returnValue = defaultValue;
    switch (value.index())
    {
    case plagn::INT:
        return static_cast<int64_t>(get<int>(value));
    case plagn::UINT:
        return static_cast<int64_t>(get<unsigned int>(value));
    case plagn::INT64:
        return get<int64_t>(value);
    case plagn::UINT64:
        if (get<uint64_t>(value) > INT64_MAX) return defaultValue;
        return static_cast<int64_t>(get<uint64_t>(value));
    case plagn::DOUBLE:
        {
            double val = get<double>(value);
            if (val > INT64_MAX || val < INT64_MIN) return defaultValue;
            return static_cast<int64_t>(val);
        }
    case plagn::STRING:
        {
            string valueAsStr = get<string>(value);
            if (valueAsStr == ""
                || (!isDigit(valueAsStr.front())
                    && static_cast<int>(valueAsStr.front()) != 0x2D)
                || (static_cast<int>(valueAsStr.front()) == 0x2D
                    && (valueAsStr.size() < 2
                        || !isDigit(valueAsStr[1]))))
            {
                throw std::invalid_argument("Expected number. Got string!");
            }
            return stoll(valueAsStr, nullptr, 0);
        }
    case plagn::MAP:
        {
            map<string, string> valueAsMap = get<map<string, string>>(value);
            if (valueAsMap.count("value") > 0) return convertDataTypeToInt64(valueAsMap["value"]);
            throw std::invalid_argument("Did not find map entry for conversion to Int64!");
        }
    case plagn::VECTOR:
        {
            vector<string> valueAsVector = get<vector<string>>(value);
            if (valueAsVector.size() > 0) return convertDataTypeToInt64(valueAsVector.front());
            throw std::invalid_argument("Cannot use empty vector for conversion to Int64!");
        }
    }
    return returnValue;
}
catch (exception & e)
{
    throw std::runtime_error("happened here");
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief extracts or converts a value of unspecified DataType into an unsigned 64-bit integer
 *
 * @param value instane of a std::variant (=typedef'ed to DataType)
 * @param defaultValue what this value will equate to, for some errors
 * @return uint64_t
 * @remark When to throw an exception and when to use @p defaultValue has no clear rules, yet
 */
uint64_t convertDataTypeToUint64(const DataType & value, uint64_t defaultValue) try
{
    uint64_t returnValue = defaultValue;
    switch (value.index())
    {
    case plagn::INT:
        {
            int valAsInt = get<int>(value);
            if (valAsInt < 0) throw std::invalid_argument("Can't convert numbers < 0 to unsigned!");
            return static_cast<uint64_t>(valAsInt);
        }
    case plagn::UINT:
        return static_cast<uint64_t>(get<unsigned int>(value));
    case plagn::INT64:
        {
            int64_t val = get<int64_t>(value);
            if (val < 0) throw std::invalid_argument("Can't convert numbers < 0 to unsigned!");
            return static_cast<uint64_t>(val);
        }
    case plagn::UINT64:
        return get<uint64_t>(value);
    case plagn::DOUBLE:
        {
            double val = get<double>(value);
            if (val < 0) throw std::invalid_argument("Can't convert numbers < 0 to unsigned!");
            if (val > UINT64_MAX) return defaultValue;
            return static_cast<uint64_t>(val);
        }
    case plagn::STRING:
        {
            string valueAsStr = get<string>(value);
            if (valueAsStr == ""
                || (!isDigit(valueAsStr.front())
                    && static_cast<int>(valueAsStr.front()) != 0x2D))
            {
                throw std::invalid_argument("Expected unsigned number. Got string!");
            }
            return stoull(valueAsStr, nullptr, 0);
        }
    case plagn::MAP:
        {
            map<string, string> valueAsMap = get<map<string, string>>(value);
            if (valueAsMap.count("value") > 0) return convertDataTypeToUint64(valueAsMap["value"]);
            throw std::invalid_argument("Did not find map entry for conversion to Uint!");
        }
    case plagn::VECTOR:
        {
            vector<string> valueAsVector = get<vector<string>>(value);
            if (valueAsVector.size() > 0) return convertDataTypeToUint64(valueAsVector.front());
            throw std::invalid_argument("Cannot use empty vector for conversion to Uint!");
        }
    }
    return returnValue;
}
catch (exception & e)
{
    throw std::runtime_error("happened here");
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief extracts or converts a value of unspecified DataType into a double (floating point)
 *
 * @param value instane of a std::variant (=typedef'ed to DataType)
 * @param defaultValue what this value will equate to, for some errors
 * @return double
 * @remark When to throw an exception and when to use @p defaultValue has no clear rules, yet. Also
 * note, that as the string conversion might be ugly, your best bet is to transport the value as a
 * string.
 */
double convertDataTypeToDouble(const DataType & value, double defaultValue) try
{
    double returnValue = defaultValue;
    switch (value.index())
    {
    case plagn::INT:
        return static_cast<double>(get<int>(value));
    case plagn::UINT:
        return static_cast<double>(get<unsigned int>(value));
    case plagn::INT64:
        return static_cast<double>(get<int64_t>(value));
    case plagn::UINT64:
        return static_cast<double>(get<uint64_t>(value));
    case plagn::DOUBLE:
        return get<double>(value);
    case plagn::STRING:
        {
            string valueAsStr = get<string>(value);
            if (valueAsStr == ""
                || (!isDigit(valueAsStr.front())
                    && static_cast<int>(valueAsStr.front()) != 0x2D)
                || (static_cast<int>(valueAsStr.front()) == 0x2D
                    && (valueAsStr.size() < 2
                        || !isDigit(valueAsStr[1]))))
            {
                throw std::invalid_argument("Expected floating point number. Got string!");
            }
            return stod(valueAsStr, nullptr);
        }
    case plagn::MAP:
        {
            map<string, string> valueAsMap = get<map<string, string>>(value);
            if (valueAsMap.count("value") > 0) return convertDataTypeToDouble(valueAsMap["value"]);
            throw std::invalid_argument("Did not find map entry for conversion to Uint!");
        }
    case plagn::VECTOR:
        {
            vector<string> valueAsVector = get<vector<string>>(value);
            if (valueAsVector.size() > 0) return convertDataTypeToDouble(valueAsVector.front());
            throw std::invalid_argument("Cannot use empty vector for conversion to Uint!");
        }
    }
    return returnValue;
}
catch (exception & e)
{
    throw std::runtime_error("happened here");
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief extracts or converts a value of unspecified DataType into a string
 *
 * @param value instane of a std::variant (=typedef'ed to DataType)
 * @return string
 * @remark The correct evluation of floating point numbers is somewhat unclear. We do not want
 * trailing zeroes, as we do not stupid rounding errors. Problem remains to be solved.
 */
string convertDataTypeToString(const DataType & value) try
{
    switch (value.index())
    {
    case plagn::INT:
        return to_string(get<int>(value));
    case plagn::UINT:
        return to_string(get<unsigned int>(value));
    case plagn::INT64:
        return to_string(get<int64_t>(value));
    case plagn::UINT64:
        return to_string(get<uint64_t>(value));
    case plagn::DOUBLE:
        return to_string(get<double>(value));
    case plagn::STRING:
        return get<string>(value);
    case plagn::MAP:
        {
            map<string, string> valueAsMap = get<map<string, string>>(value);
            return to_string(valueAsMap);
        }
    case plagn::VECTOR:
        {
            vector<string> valueAsVector = get<vector<string>>(value);
            return to_string(valueAsVector);
        }
    }
    return "";
}
catch (exception & e)
{
    throw std::runtime_error("happened here");
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief extracts or converts a value of unspecified DataType into a std::map<string, string>
 *
 * @param value instane of a std::variant (=typedef'ed to DataType)
 * @return std::map<string, string>
 * @remark Attention to re-conversion was given. Map to string and back should work, as should
 * map to vector and back. However, I cannot guarantee this to be working in a triangle of string,
 * map and vector.
 */
map<string, string> convertDataTypeToMap(const DataType & value) try
{
    map<string, string> conversion;
    string defaultKey = "value";
    switch (value.index())
    {
    case plagn::INT:
        conversion.insert_or_assign(defaultKey, to_string(get<int>(value)));
        break;
    case plagn::UINT:
        conversion.insert_or_assign(defaultKey, to_string(get<unsigned int>(value)));
        break;
    case plagn::INT64:
        conversion.insert_or_assign(defaultKey, to_string(get<int64_t>(value)));
        break;
    case plagn::UINT64:
        conversion.insert_or_assign(defaultKey, to_string(get<uint64_t>(value)));
        break;
    case plagn::DOUBLE:
        conversion.insert_or_assign(defaultKey, to_string(get<double>(value)));
        break;
    case plagn::STRING:
        {
            string valueAsStr = get<string>(value);
            if (startsWith('{', valueAsStr) && endsWith('}', valueAsStr)
                && valueAsStr.find('\"') != string::npos && valueAsStr.find(':') != string::npos)
            {
                // this is likely a stringified map
                size_t startKeyPos = valueAsStr.find('\"') + 1;
                size_t endKeyPos = valueAsStr.find('\"', startKeyPos);
                size_t colonPos = valueAsStr.find(':', startKeyPos);
                size_t startValuePos = valueAsStr.find('\"', colonPos) + 1;
                size_t endValuePos = valueAsStr.find('\"', startValuePos);
                while (startKeyPos != string::npos && endKeyPos < colonPos
                       && colonPos != string::npos
                       && startValuePos != string::npos && endValuePos != string::npos)
                {
                    string key = valueAsStr.substr(startKeyPos, endKeyPos - startKeyPos);
                    string value = valueAsStr.substr(startValuePos, endValuePos - startValuePos);
                    conversion.insert_or_assign(key, value);
                    startKeyPos = valueAsStr.find('\"', endValuePos + 1) + 1;
                    endKeyPos = valueAsStr.find('\"', startKeyPos);
                    colonPos = valueAsStr.find(':', startKeyPos);
                    startValuePos = valueAsStr.find('\"', colonPos) + 1;
                    endValuePos = valueAsStr.find('\"', startValuePos);
                }
            }
            else if (startsWith('[', valueAsStr) && endsWith(']', valueAsStr)
                     && valueAsStr.find('\"') != string::npos && valueAsStr.find(',') != string::npos)
            {
                // this is likely a stringified vector
                vector<string> extractedVector = convertDataTypeToVector(value);
                return convertDataTypeToMap(extractedVector);
            }
            else
            {
                conversion.insert_or_assign(defaultKey, valueAsStr);
            }
            break;
        }
    case plagn::MAP:
        return get<map<string, string>>(value);
    case plagn::VECTOR:
        {
            vector<string> valueAsVector = get<vector<string>>(value);
            if (valueAsVector.size() == 0) throw std::invalid_argument("Cannot turn empty vector to map.");
            // using the first one to detect, wheter the vector contains a nested map
            // this is not guaranteed to work flawlessly. This more a
            // "60 % of the time, it works 100 % of the time"-thing.
            bool hasNestedMap = (startsWith('{', valueAsVector.front())
                                && endsWith('}', valueAsVector.front())
                                && valueAsVector.front().find('\"') != string::npos
                                && valueAsVector.front().find(':') != string::npos);
            if (hasNestedMap)
            {
                for (const string & entry : valueAsVector)
                {
                    size_t startKeyPos = entry.find('\"') + 1;
                    size_t endKeyPos = entry.find('\"', startKeyPos);
                    size_t colonPos = entry.find(':', startKeyPos);
                    size_t startValuePos = entry.find('\"', colonPos) + 1;
                    size_t endValuePos = entry.find('\"', startValuePos);
                    string key = entry.substr(startKeyPos, endKeyPos - startKeyPos);
                    string value = entry.substr(startValuePos, endValuePos - startValuePos);
                    conversion.insert_or_assign(key, value);
                }
            }
            else
            {
                for (size_t i = 0; i < valueAsVector.size(); i++)
                {
                    string key = string("value") + to_string(i + 1);
                    conversion.insert_or_assign(key, valueAsVector.at(i));
                }
            }
        }
    }
    return conversion;
}
catch (exception & e)
{
    throw std::runtime_error("happened here");
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief extracts or converts a value of unspecified DataType into a vector<string>
 *
 * @param value instane of a std::variant (=typedef'ed to DataType)
 * @return vector<string>
 * @remark Attention to re-conversion was given. Map to string and back should work, as should
 * map to vector and back. However, I cannot guarantee this to be working in a triangle of string,
 * map and vector.
 */
vector<string> convertDataTypeToVector(const DataType & value) try
{
    vector<string> conversion;
    switch (value.index())
    {
    case plagn::INT:
        conversion.push_back(to_string(get<int>(value)));
        break;
    case plagn::UINT:
        conversion.push_back(to_string(get<unsigned int>(value)));
        break;
    case plagn::INT64:
        conversion.push_back(to_string(get<int64_t>(value)));
        break;
    case plagn::UINT64:
        conversion.push_back(to_string(get<uint64_t>(value)));
        break;
    case plagn::DOUBLE:
        conversion.push_back(to_string(get<double>(value)));
        break;
    case plagn::STRING:
    {
        string valueAsStr = get<string>(value);
        if (startsWith('{', valueAsStr) && endsWith('}', valueAsStr)
            && valueAsStr.find('\"') != string::npos && valueAsStr.find(':') != string::npos)
        {
            // this is likely a stringified map
            map<string, string> valueAsMap = convertDataTypeToMap(value);
            return convertDataTypeToVector(valueAsMap);
        }
        else if (startsWith('[', valueAsStr) && endsWith(']', valueAsStr)
                 && valueAsStr.find('\"') != string::npos && valueAsStr.find(',') != string::npos)
        {
            // this is likely a stringified vector
            size_t startValuePos = valueAsStr.find('\"') + 1;
            size_t endValuePos = valueAsStr.find('\"', startValuePos);
            size_t commaPos = valueAsStr.find(',', startValuePos);
            while (startValuePos != string::npos && endValuePos < commaPos
                   && commaPos != string::npos)
            {
                string value = valueAsStr.substr(startValuePos, endValuePos - startValuePos);
                conversion.push_back(value);
                startValuePos = valueAsStr.find('\"', endValuePos + 1) + 1;
                endValuePos = valueAsStr.find('\"', startValuePos);
                commaPos = valueAsStr.find(',', startValuePos);
            }
        }
        else
        {
            conversion.push_back(valueAsStr);
        }
        break;
    }
    case plagn::MAP:
        {
            map<string, string> valueAsMap = get<map<string, string>>(value);
            bool isMapfiedVector = (valueAsMap.begin()->first.find("value") == 0
                                    && valueAsMap.end()->first.find("value") == 0);
            if (isMapfiedVector)
            {
                for (size_t i = 0; i < valueAsMap.size(); i++)
                {
                    string key = string("value") + to_string(i);
                    if (valueAsMap.count(key) == 1)
                    {
                        conversion.push_back(valueAsMap.at(key));
                    }
                    else
                    {
                        throw std::runtime_error("Incomplete mapified vector!");
                    }
                }
            }
            else
            {
                for (const pair<string, string> & keyValPair : valueAsMap)
                {
                    conversion.push_back(string("{") + keyValPair.first + ":" + keyValPair.second + "}");
                }
            }
        }
        break;
    case plagn::VECTOR:
        return get<vector<string>>(value);
    }
    return conversion;
}
catch (exception & e)
{
    throw std::runtime_error("happened here");
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief evaluates wheter a given character is a digit (number)
 * 
 * @param c the chracter to test
 * @return true when c's ascii index is with 0x30 and 0x39
 * @return false else
 */
bool isDigit(char c) noexcept
{
    unsigned int asciiIndex = static_cast<unsigned int>(c);
    return (asciiIndex >= 0x30 && asciiIndex <= 0x39);
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief convenience function to test, wheter a string starts with a certain character
 * 
 * @param c the character we want to test against
 * @param text the string, that eiter starts with @p c or does not
 * @return true if the first char of @p text is @p c
 * @return false else
 */
bool startsWith(char c, const std::string & text)
{
    return (text.size() > 0 && text.at(0) == c);
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief convenience function to test, wheter a string ends with a certain character
 *
 * @param c the character we want to test against
 * @param text the string, that eiter ends with @p c or does not
 * @return true if the last char of @p text is @p c
 * @return false else
 */
bool endsWith(char c, const std::string & text)
{
    return (text.size() > 0 && text.at(text.size() - 1) == c);
}

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

/**
 *-------------------------------------------------------------------------------------------------
 * @brief stream-operator overload to get consistent string representation of one of our
 * std::variant types
 *
 * @param os stream, the operators "<<" refer to 
 * @param strVector a vector<string> to be printed as a string
 * @return ostream& 
 */
ostream & operator<<(ostream & os, const vector<string> & strVector)
{
    os << "[";
    bool isFirst = true;
    for (const string & txt : strVector)
    {
        os << (isFirst ? "\"" : "\", \"") << txt;
    }
    return os << "\"]";
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief stream-operator overload to get consistent string representation of one of our
 * std::variant types
 *
 * @param os stream, the operators "<<" refer to
 * @param strMap a map<string, string> to be printed as a string
 * @return ostream&
 */
ostream & operator<<(ostream & os, const map<string, string> & strMap)
{
    os << "{";
    bool isFirst = true;
    for (const pair<string, string> & item : strMap)
    {
        os << (isFirst ? "\"" : ", \"") << item.first << "\":\"" << item.second << "\"";
    }
    return os << "}";
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief to_string overload to get consistent string representation of one of our std::variant types
 *
 * @param strVector a vector<string> to be printed as a string
 * @return string
 * @remark uses the stream operator implementation
 * @sa ostream & operator<<(ostream & os, const vector<string> & strVector)
 */
string to_string(const vector<string> & strVector)
{
    stringstream stream;
    stream << strVector;
    return stream.str();
}


/**
 *-------------------------------------------------------------------------------------------------
 * @brief to_string overload to get consistent string representation of one of our std::variant types
 *
 * @param strVector a map<string, string> to be printed as a string
 * @return string
 * @remark uses the stream operator implementation
 * @sa ostream & operator<<(ostream & os, const map<string, string> & strMap)
 */
string to_string(const map<string, string> & strMap)
{
    stringstream stream;
    stream << strMap;
    return stream.str();
}