--[[
 -------------------------------------------------------------------------------------------------
 @file plagHttpServerUtils.lua
 @author Fabian Köslin (fabian@koeslin.info)
 @contributors:
 @brief Holds some utility functions for the plagHttpServer Lua
 @version 0.1
 @date 2022-12-23
 
 @copyright LGPL v2.1
 
 Targets of chosen license for:
      Users    : Please be so kind as to indicate your usage of this library by linking to the project
                 page, currently being: https://github.com/saxomophon/plagn
      Devs     : Your improvements to the code, should be available publicly under the same license.
                 That way, anyone will benefit from it.
      Corporate: Even you are either a User or a Developer. No charge will apply, no guarantee or
                 warranty will be given.
]]

-- some predefined HTTP status codes
local HTTP_100 = { 100, "Continue" };
local HTTP_101 = { 101, "Switching Protocols" };
local HTTP_103 = { 103, "Early Hints" };
local HTTP_200 = { 200, "OK" };
local HTTP_201 = { 201, "Created" };
local HTTP_202 = { 202, "Accepted" };
local HTTP_203 = { 203, "Non - Authoritative Information" };
local HTTP_204 = { 204, "No Content" };
local HTTP_205 = { 205, "Reset Content" };
local HTTP_206 = { 206, "Partial Content" };
local HTTP_300 = { 300, "Multiple Choices" };
local HTTP_301 = { 301, "Moved Permanently" };
local HTTP_302 = { 302, "Found" };
local HTTP_303 = { 303, "See Other" };
local HTTP_304 = { 304, "Not Modified" };
local HTTP_307 = { 307, "Temporary Redirect" };
local HTTP_308 = { 308, "Permanent Redirect" };
local HTTP_400 = { 400, "Bad Request" };
local HTTP_401 = { 401, "Unauthorized" };
local HTTP_402 = { 402, "Payment Required" };
local HTTP_403 = { 403, "Forbidden" };
local HTTP_404 = { 404, "Not Found" };
local HTTP_405 = { 405, "Method Not Allowed" };
local HTTP_406 = { 406, "Not Acceptable" };
local HTTP_407 = { 407, "Proxy Authentication Required" };
local HTTP_408 = { 408, "Request Timeout" };
local HTTP_409 = { 409, "Conflict" };
local HTTP_410 = { 410, "Gone" };
local HTTP_411 = { 411, "Length Required" };
local HTTP_412 = { 412, "Precondition Failed" };
local HTTP_413 = { 413, "Payload Too Large" };
local HTTP_414 = { 414, "URI Too Long" };
local HTTP_415 = { 415, "Unsupported Media Type" };
local HTTP_416 = { 416, "Range Not Satisfiable" };
local HTTP_417 = { 417, "Expectation Failed" };
local HTTP_418 = { 418, "I'm a teapot" };
local HTTP_425 = { 425, "Too Early" };
local HTTP_426 = { 426, "Upgrade Required" };
local HTTP_428 = { 428, "Precondition Required" };
local HTTP_429 = { 429, "Too Many Requests" };
local HTTP_431 = { 431, "Request Header Fields Too Large" };
local HTTP_451 = { 451, "Unavailable For Legal Reasons" };
local HTTP_500 = { 500, "Internal Server Error" };
local HTTP_501 = { 501, "Not Implemented" };
local HTTP_502 = { 502, "Bad Gateway" };
local HTTP_503 = { 503, "Service Unavailable" };
local HTTP_504 = { 504, "Gateway Timeout" };
local HTTP_505 = { 505, "HTTP Version Not Supported" };
local HTTP_506 = { 506, "Variant Also Negotiates" };
local HTTP_510 = { 510, "Not Extended" };
local HTTP_511 = { 511, "Network Authentication Required" };