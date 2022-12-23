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

-- I'm only a example
print("[LUA] Hello from LUA")

print("Header:")
for key,val in pairs(reqHeader) do
    print(key .. ": " .. val)
end 

print("Params:")
for key,val in pairs(reqParams) do
    print(key .. ": " .. val)
end 

print("Content: " .. reqContent)

print("Endpoint: " .. reqEndpoint)

print("Http Version: " .. reqHttpVersion)

-- set the header for resp
respHeader = {}
respHeader["Content-Type"] = "text/html; charset=utf-8"

-- set the resp content 
respContent = [[<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hello World from Plagn</title>
  </head>
  <body>
    <h1>Hello World from Plagn!</h1>
  </body>
</html>]]

-- set the resp return code and message
respStatus = {}
respStatus["code"] = 200
respStatus["message"] = "OK"

-- test the functions
data = {}
data["cmd"] = "sendInfo"
data["info"] = "test message"
sendDatagram(data)

retData = resvDatagram()
print("------------------------")
print("PlagData:")
for key,val in pairs(retData) do
    print(key .. ": " .. val)
end 
print("------------------------")