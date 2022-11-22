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

// openssl includes
#include <openssl/evp.h>

// own includes
#include "Utilities.hpp"

// self include
#include "Datagram.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Datagram:: Datagram object sets member values
 * 
 * @param sourceName the name of the Plag, which created this Datagram
 */
Datagram::Datagram(const string & sourceName) :
    m_sourceDatagramId(0),
    m_timeOfCreation(std::chrono::steady_clock::now())
{
    //TODO: create a unique hash of some sort
    string nowStr;
    nowStr = getTimeAsUtcIsoStr(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    string feedFeed = sourceName + to_string(m_sourceDatagramId);
    uint32_t remainder;
    std::chrono::microseconds sinceEpoch;
    sinceEpoch = std::chrono::duration_cast<std::chrono::microseconds>(m_timeOfCreation.time_since_epoch());
    remainder = sinceEpoch.count() % 1000000;
    feedFeed += nowStr;
    feedFeed += to_string(remainder);

    const unsigned char * feed = reinterpret_cast<const unsigned char *>(feedFeed.c_str());

    // creating a unique identifier hash using openssl's evp
    EVP_MD_CTX * md5Context;
    unsigned char * md5Result;
    unsigned int md5ResultLength = EVP_MD_size(EVP_md5());

    // MD5_Init
    md5Context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md5Context, EVP_md5(), nullptr);

    // MD5_Update
    EVP_DigestUpdate(md5Context, feed, feedFeed.size());

    // MD5_Final
    md5Result = reinterpret_cast<unsigned char *>(OPENSSL_malloc(md5ResultLength));
    EVP_DigestFinal_ex(md5Context, md5Result, &md5ResultLength);
    EVP_MD_CTX_free(md5Context);

    m_ownId = string(reinterpret_cast<char *>(md5Result));

    //cout << "My ID is " << m_ownId << endl;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to access data of Datagram under just one name
 * 
 * @param key reference name of the value to access
 * @return DataType 
 */
DataType Datagram::getData(const string & key) const
{
    if ((startsWith('\"', key) && endsWith('\"', key)) || isDigit(key.front())) // reading a literal
    {
        if (isDigit(key.front()))
        {
            return key;
        }
        return key.substr(1, key.size() - 2);
    }
    else if (key == string("uuid"))
    {
        return m_ownId;
    }
    else if (key == string("sourceDatagramId"))
    {
        return m_sourceDatagramId;
    }
    else
    {
        throw std::invalid_argument(string("Invalid key \"") + key + "\"");
    }
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief method to write data of Datagram under just one name
 *
 * @param key reference name of the value to override
 * @param value value to bet set for member accssible by @p key
 */
void Datagram::setData(const string & key, const DataType & value)
{
    if (key == string("sourceDatagramId"))
    {
        m_sourceDatagramId = get<uint64_t>(value);
    }
    else
    {
        throw std::invalid_argument(string("Invalid key \"") + key + "\"");
    }
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
    if (m_sourceDatagramId == 0) stringRepresentation += "; newly generated";
    //TODO: string representation of time
    //stringRepresentation += ";at " + to_string(m_timeOfCreation);
    stringRepresentation += "; with id " + m_ownId;
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
