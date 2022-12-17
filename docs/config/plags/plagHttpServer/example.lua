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