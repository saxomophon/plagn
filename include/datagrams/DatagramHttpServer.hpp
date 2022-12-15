/**
 *-------------------------------------------------------------------------------------------------
 * @file DatagramHttpServer.hpp
 * @author Fabian Köslin (fabian@koeslin.info)
 * @contributors:
 * @brief Holds the DatagramHttpServer class
 * @version 0.1
 * @date 2022-12-15
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

#ifndef DATAGRAMHTTPSERVER_HPP
#define DATAGRAMHTTPSERVER_HPP

// std includes

// own includes
#include "DatagramMap.hpp"

class DatagramHttpServer : public DatagramMap
{
public:
    DatagramHttpServer(const std::string & sourcePlag);
    DatagramHttpServer(const std::string & sourcePlag,
                std::string reqId, std::map<std::string, DataType> map);

    std::string getReqId();

    void setReqId(std::string reqId);

    virtual DataType getData(const std::string & key) const;

    virtual void setData(const std::string & key, const DataType & value);

    virtual std::string toString() const;

private:
    std::string m_reqId;
};

#endif // DATAGRAMHTTPSERVER_HPP