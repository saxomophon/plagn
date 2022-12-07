-- I'm only a example
print("[LUA] Hellp from LUA")

print("Header:")
for key,val in pairs(header) do
    print(key .. ": " .. val)
end 

print("Params:")
for key,val in pairs(params) do
    print(key .. ": " .. val)
end 

print("Content: " .. content)

print("Endpoint: " .. endpoint)

print("Http Version: " .. httpversion)

print("Method: " .. method)