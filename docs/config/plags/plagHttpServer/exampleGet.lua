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
package.path = '../docs/config/plags/plagHttpServer/luaModules/?.lua;' .. package.path
require("httpResponseCodes")
require("httpContentUtils")
require("httpContentTypes")

-- initalize the respHeader
respHeader = {}

print("Endpoint: " .. reqEndpoint)

-- set reqEntpoint to "/index.html" if it is "/"
if reqEndpoint == "/" then
    reqEndpoint = "/index.html"
end

-- set the filename for the resp content
local filename = reqWorkingDir .. reqEndpoint

-- check if the file exists and return 404 if not, else return the content from the file and set the status to 200
if not httpContentUtils.file_exists(filename) then
    respStatus = HTTP_404
    respContent = "404 - File not found"
    return
else
    respContent, fileExtension = httpContentUtils.get_file_content(filename)
    respStatus = HTTP_200
    respHeader["Content-Type"] = httpContentTypes.get(fileExtension)
    return
end
