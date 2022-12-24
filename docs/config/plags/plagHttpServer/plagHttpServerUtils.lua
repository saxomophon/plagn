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
HTTP_100 = { code = 100, message = " Continue" }
HTTP_101 = { code = 101, message = " Switching Protocols" }
HTTP_103 = { code = 103, message = " Early Hints" }
HTTP_200 = { code = 200, message = " OK" }
HTTP_201 = { code = 201, message = " Created" }
HTTP_202 = { code = 202, message = " Accepted" }
HTTP_203 = { code = 203, message = " Non - Authoritative Information" }
HTTP_204 = { code = 204, message = " No Content" }
HTTP_205 = { code = 205, message = " Reset Content" }
HTTP_206 = { code = 206, message = " Partial Content" }
HTTP_300 = { code = 300, message = " Multiple Choices" }
HTTP_301 = { code = 301, message = " Moved Permanently" }
HTTP_302 = { code = 302, message = " Found" }
HTTP_303 = { code = 303, message = " See Other" }
HTTP_304 = { code = 304, message = " Not Modified" }
HTTP_307 = { code = 307, message = " Temporary Redirect" }
HTTP_308 = { code = 308, message = " Permanent Redirect" }
HTTP_400 = { code = 400, message = " Bad Request" }
HTTP_401 = { code = 401, message = " Unauthorized" }
HTTP_402 = { code = 402, message = " Payment Required" }
HTTP_403 = { code = 403, message = " Forbidden" }
HTTP_404 = { code = 404, message = " Not Found" }
HTTP_405 = { code = 405, message = " Method Not Allowed" }
HTTP_406 = { code = 406, message = " Not Acceptable" }
HTTP_407 = { code = 407, message = " Proxy Authentication Required" }
HTTP_408 = { code = 408, message = " Request Timeout" }
HTTP_409 = { code = 409, message = " Conflict" }
HTTP_410 = { code = 410, message = " Gone" }
HTTP_411 = { code = 411, message = " Length Required" }
HTTP_412 = { code = 412, message = " Precondition Failed" }
HTTP_413 = { code = 413, message = " Payload Too Large" }
HTTP_414 = { code = 414, message = " URI Too Long" }
HTTP_415 = { code = 415, message = " Unsupported Media Type" }
HTTP_416 = { code = 416, message = " Range Not Satisfiable" }
HTTP_417 = { code = 417, message = " Expectation Failed" }
HTTP_418 = { code = 418, message = " I'm a teapot" }
HTTP_425 = { code = 425, message = " Too Early" }
HTTP_426 = { code = 426, message = " Upgrade Required" }
HTTP_428 = { code = 428, message = " Precondition Required" }
HTTP_429 = { code = 429, message = " Too Many Requests" }
HTTP_431 = { code = 431, message = " Request Header Fields Too Large" }
HTTP_451 = { code = 451, message = " Unavailable For Legal Reasons" }
HTTP_500 = { code = 500, message = " Internal Server Error" }
HTTP_501 = { code = 501, message = " Not Implemented" }
HTTP_502 = { code = 502, message = " Bad Gateway" }
HTTP_503 = { code = 503, message = " Service Unavailable" }
HTTP_504 = { code = 504, message = " Gateway Timeout" }
HTTP_505 = { code = 505, message = " HTTP Version Not Supported" }
HTTP_506 = { code = 506, message = " Variant Also Negotiates" }
HTTP_510 = { code = 510, message = " Not Extended" }
HTTP_511 = { code = 511, message = " Network Authentication Required" }