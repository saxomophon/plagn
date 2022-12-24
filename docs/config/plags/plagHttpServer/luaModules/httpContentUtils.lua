--[[
 -------------------------------------------------------------------------------------------------
 @file plagHttpServerUtils.lua
 @author Fabian Köslin (fabian@koeslin.info)
 @contributors:
 @brief Holds http content utils
 @version 0.1
 @date 2022-12-24
 
 @copyright LGPL v2.1
 
 Targets of chosen license for:
      Users    : Please be so kind as to indicate your usage of this library by linking to the project
                 page, currently being: https://github.com/saxomophon/plagn
      Devs     : Your improvements to the code, should be available publicly under the same license.
                 That way, anyone will benefit from it.
      Corporate: Even you are either a User or a Developer. No charge will apply, no guarantee or
                 warranty will be given.
]]

httpContentUtils = {}

-- @brief Returns if a file exists
function httpContentUtils.file_exists(filename)
    local file = io.open(filename, "rb")
    if file then file:close() end
    return file ~= nil
end

-- @brief Returns the content of a given file and the fileextension without the dot
function httpContentUtils.get_file_content(filename)
    local file = io.open(filename, "rb")
    local content = file:read("*all")
    file:close()
    local fileExtension = string.match(filename, "%.(%w+)$")
    return content, fileExtension
end

-- @brief parse a json string and return a table with the data
function httpContentUtils.parse_json(jsonString)
    local json = require("json")
    local jsonTable = json.decode(jsonString)
    return jsonTable
end

-- @brief parse a table and return a json string
function httpContentUtils.parse_table(jsonTable)
    local json = require("json")
    local jsonString = json.encode(jsonTable)
    return jsonString
end

-- @brief parse a http from data and return a table with the data
function httpContentUtils.parse_http_form_data(httpFormData)
    local httpFormDataTable = {}
    for key, value in string.gmatch(httpFormData, "([^&=]+)=([^&=]+)") do
        httpFormDataTable[key] = value
    end
    return httpFormDataTable
end

-- @brief parse a table and return a http form data string
function httpContentUtils.parse_table_to_http_form_data(httpFormDataTable)
    local httpFormData = ""
    for key, value in pairs(httpFormDataTable) do
        httpFormData = httpFormData .. key .. "=" .. value .. "&"
    end
    return httpFormData
end