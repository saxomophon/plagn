--[[
 -------------------------------------------------------------------------------------------------
 @file exampleGet.lua
 @author Fabian Köslin (fabian@koeslin.info)
 @contributors:
 @brief Holds a example for a POST (Form Data) request
 @version 0.1
 @date 2022-12-27
 
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
package.path = '../docs/config/plags/plagHttpServer/luaModules/?.lua;' .. package.path
require("httpResponseCodes")
require("httpContentUtils")
require("httpContentTypes")

-- initalize the respHeader
respHeader = {}

print("Endpoint: " .. reqEndpoint)

-- parse the content
local httpFormDataTable = httpContentUtils.parse_http_form_data(reqContent)

-- check if httpFormDataTable has keys "message" and "recipient"
if httpFormDataTable["message"] == nil or httpFormDataTable["recipient"] == nil then
    -- set the response status to HTTP_400
    respStatus = HTTP_400
    -- set the content type to text/plain
    respHeader["Content-Type"] = httpContentTypes.get("text")
    -- set the response content
    respContent = "Bad Request"
    return
end

-- send the message via plagn
local sendingTable = {}
sendingTable["message"] = httpFormDataTable["message"]
sendingTable["recipient"] = httpFormDataTable["recipient"]
sendDatagram(sendingTable)

-- set up the resp table and encode it to http form data
local respTable = {}
respTable["status"] = "ok"
respContent = httpContentUtils.parse_table_to_http_form_data(respTable)

-- set the content type to x-www-form-urlencoded
respHeader["Content-Type"] = httpContentTypes.get("x-www")

-- set the response status to HTTP_200
respStatus = HTTP_200

