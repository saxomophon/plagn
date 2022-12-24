--[[
 -------------------------------------------------------------------------------------------------
 @file exampleGet.lua
 @author Fabian Köslin (fabian@koeslin.info)
 @contributors:
 @brief Holds a example for a GET request
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

-- require plagHttpServerUtils
package.path = '../docs/config/plags/plagHttpServer/?.lua;' .. package.path
require("plagHttpServerUtils")

print("Endpoint: " .. reqEndpoint)

-- set the header for resp
respHeader = {}
respHeader["Content-Type"] = "text/html; charset=utf-8"

-- set the resp content, read from a file 
local filename = reqWorkingDir .. "/www/index.html"

local file = assert(io.open(filename, "rb"))

respContent = file:read("*all")

file:close()

-- set the resp return code and message
respStatus = HTTP_200