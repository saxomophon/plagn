/**
 *-------------------------------------------------------------------------------------------------
 * @file AsyncHttpServerUtils.hpp
 * @author Fabian Köslin (fabian@koeslin.info)
 * @contributors:
 * @brief Holds some typedefs and consts for the async HTTP SERVER
 * @version 0.1
 * @date 2022-12-18
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

#ifndef ASYNCHTTPSERVERUTILS_HPP_
#define ASYNCHTTPSERVERUTILS_HPP_

// std includes
#include <string>

namespace AsyncHttpServerUtils
{
    typedef enum
    {
        HTTP_UNKNOWN = 0,
        HTTP_GET = 1,
        HTTP_POST = 2,
        HTTP_PUT = 3,
        HTTP_DELETE = 4,
        HTTP_HEAD = 5,
        HTTP_CONNECT = 6,
        HTTP_OPTIONS = 7,
        HTTP_TRACE = 8,
        HTTP_PATCH = 9
    } httpMethod; //<! Http Method enum

    typedef struct
    {
        std::string endpoint;
        httpMethod method;

    } endpoint_t; //<! enpoint definetion set

    typedef struct
    {
        int code;
        std::string message;
    } responseStatusCode_t; //<! HTTP response set


    // Http Response Codes and Messages 
    const responseStatusCode_t HTTP_100 = { 100, "Continue" };
    const responseStatusCode_t HTTP_101 = { 101, "Switching Protocols" };
    const responseStatusCode_t HTTP_103 = { 103, "Early Hints" };
    const responseStatusCode_t HTTP_200 = { 200, "OK" };
    const responseStatusCode_t HTTP_201 = { 201, "Created" };
    const responseStatusCode_t HTTP_202 = { 202, "Accepted" };
    const responseStatusCode_t HTTP_203 = { 203, "Non - Authoritative Information" };
    const responseStatusCode_t HTTP_204 = { 204, "No Content" };
    const responseStatusCode_t HTTP_205 = { 205, "Reset Content" };
    const responseStatusCode_t HTTP_206 = { 206, "Partial Content" };
    const responseStatusCode_t HTTP_300 = { 300, "Multiple Choices" };
    const responseStatusCode_t HTTP_301 = { 301, "Moved Permanently" };
    const responseStatusCode_t HTTP_302 = { 302, "Found" };
    const responseStatusCode_t HTTP_303 = { 303, "See Other" };
    const responseStatusCode_t HTTP_304 = { 304, "Not Modified" };
    const responseStatusCode_t HTTP_307 = { 307, "Temporary Redirect" };
    const responseStatusCode_t HTTP_308 = { 308, "Permanent Redirect" };
    const responseStatusCode_t HTTP_400 = { 400, "Bad Request" };
    const responseStatusCode_t HTTP_401 = { 401, "Unauthorized" };
    const responseStatusCode_t HTTP_402 = { 402, "Payment Required" };
    const responseStatusCode_t HTTP_403 = { 403, "Forbidden" };
    const responseStatusCode_t HTTP_404 = { 404, "Not Found" };
    const responseStatusCode_t HTTP_405 = { 405, "Method Not Allowed" };
    const responseStatusCode_t HTTP_406 = { 406, "Not Acceptable" };
    const responseStatusCode_t HTTP_407 = { 407, "Proxy Authentication Required" };
    const responseStatusCode_t HTTP_408 = { 408, "Request Timeout" };
    const responseStatusCode_t HTTP_409 = { 409, "Conflict" };
    const responseStatusCode_t HTTP_410 = { 410, "Gone" };
    const responseStatusCode_t HTTP_411 = { 411, "Length Required" };
    const responseStatusCode_t HTTP_412 = { 412, "Precondition Failed" };
    const responseStatusCode_t HTTP_413 = { 413, "Payload Too Large" };
    const responseStatusCode_t HTTP_414 = { 414, "URI Too Long" };
    const responseStatusCode_t HTTP_415 = { 415, "Unsupported Media Type" };
    const responseStatusCode_t HTTP_416 = { 416, "Range Not Satisfiable" };
    const responseStatusCode_t HTTP_417 = { 417, "Expectation Failed" };
    const responseStatusCode_t HTTP_418 = { 418, "I'm a teapot" };
    const responseStatusCode_t HTTP_425 = { 425, "Too Early" };
    const responseStatusCode_t HTTP_426 = { 426, "Upgrade Required" };
    const responseStatusCode_t HTTP_428 = { 428, "Precondition Required" };
    const responseStatusCode_t HTTP_429 = { 429, "Too Many Requests" };
    const responseStatusCode_t HTTP_431 = { 431, "Request Header Fields Too Large" };
    const responseStatusCode_t HTTP_451 = { 451, "Unavailable For Legal Reasons" };
    const responseStatusCode_t HTTP_500 = { 500, "Internal Server Error" };
    const responseStatusCode_t HTTP_501 = { 501, "Not Implemented" };
    const responseStatusCode_t HTTP_502 = { 502, "Bad Gateway" };
    const responseStatusCode_t HTTP_503 = { 503, "Service Unavailable" };
    const responseStatusCode_t HTTP_504 = { 504, "Gateway Timeout" };
    const responseStatusCode_t HTTP_505 = { 505, "HTTP Version Not Supported" };
    const responseStatusCode_t HTTP_506 = { 506, "Variant Also Negotiates" };
    const responseStatusCode_t HTTP_510 = { 510, "Not Extended" };
    const responseStatusCode_t HTTP_511 = { 511, "Network Authentication Required" };
}

#endif /*ASYNCHTTPSERVERUTILS_HPP_*/