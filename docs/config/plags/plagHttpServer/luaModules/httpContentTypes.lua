--[[
 -------------------------------------------------------------------------------------------------
 @file plagHttpServerUtils.lua
 @author Fabian Köslin (fabian@koeslin.info)
 @contributors:
 @brief Holds http content types
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

httpContentTypes = {}

httpContentTypes["html"] = "text/html; charset=utf-8"
httpContentTypes["htm"] = "text/html; charset=utf-8"
httpContentTypes["css"] = "text/css; charset=utf-8"
httpContentTypes["js"] = "text/javascript; charset=utf-8"
httpContentTypes["json"] = "application/json; charset=utf-8"
httpContentTypes["xml"] = "application/xml; charset=utf-8"
httpContentTypes["txt"] = "text/plain; charset=utf-8"
httpContentTypes["png"] = "image/png"
httpContentTypes["jpg"] = "image/jpeg"
httpContentTypes["jpeg"] = "image/jpeg"
httpContentTypes["gif"] = "image/gif"
httpContentTypes["svg"] = "image/svg+xml"
httpContentTypes["ico"] = "image/x-icon"
httpContentTypes["pdf"] = "application/pdf"
httpContentTypes["zip"] = "application/zip"
httpContentTypes["rar"] = "application/x-rar-compressed"
httpContentTypes["7z"] = "application/x-7z-compressed"
httpContentTypes["mp3"] = "audio/mpeg"
httpContentTypes["wav"] = "audio/wav"
httpContentTypes["ogg"] = "audio/ogg"
httpContentTypes["mp4"] = "video/mp4"
httpContentTypes["webm"] = "video/webm"
httpContentTypes["mkv"] = "video/x-matroska"
httpContentTypes["avi"] = "video/x-msvideo"
httpContentTypes["flv"] = "video/x-flv"
httpContentTypes["wmv"] = "video/x-ms-wmv"
httpContentTypes["mov"] = "video/quicktime"
httpContentTypes["swf"] = "application/x-shockwave-flash"
httpContentTypes["exe"] = "application/x-msdownload"
httpContentTypes["msi"] = "application/x-msdownload"
httpContentTypes["cab"] = "application/vnd.ms-cab-compressed"
httpContentTypes["doc"] = "application/msword"
httpContentTypes["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document"
httpContentTypes["xls"] = "application/vnd.ms-excel"
httpContentTypes["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"
httpContentTypes["ppt"] = "application/vnd.ms-powerpoint"
httpContentTypes["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation"
httpContentTypes["odt"] = "application/vnd.oasis.opendocument.text"
httpContentTypes["ods"] = "application/vnd.oasis.opendocument.spreadsheet"
httpContentTypes["odp"] = "application/vnd.oasis.opendocument.presentation"
httpContentTypes["odg"] = "application/vnd.oasis.opendocument.graphics"
httpContentTypes["odc"] = "application/vnd.oasis.opendocument.chart"
httpContentTypes["odf"] = "application/vnd.oasis.opendocument.formula"
httpContentTypes["odft"] = "application/vnd.oasis.opendocument.formula-template"
httpContentTypes["odg"] = "application/vnd.oasis.opendocument.graphics"
httpContentTypes["odm"] = "application/vnd.oasis.opendocument.text-master"
httpContentTypes["odp"] = "application/vnd.oasis.opendocument.presentation"
httpContentTypes["ods"] = "application/vnd.oasis.opendocument.spreadsheet"
httpContentTypes["odt"] = "application/vnd.oasis.opendocument.text"
httpContentTypes["oga"] = "audio/ogg"
httpContentTypes["ogg"] = "audio/ogg"
httpContentTypes["ogv"] = "video/ogg"
httpContentTypes["ogx"] = "application/ogg"
httpContentTypes["otg"] = "application/vnd.oasis.opendocument.graphics-template"
httpContentTypes["oth"] = "application/vnd.oasis.opendocument.text-web"
httpContentTypes["otp"] = "application/vnd.oasis.opendocument.presentation-template"
httpContentTypes["ots"] = "application/vnd.oasis.opendocument.spreadsheet-template"
httpContentTypes["ott"] = "application/vnd.oasis.opendocument.text-template"
httpContentTypes["rtf"] = "application/rtf"
httpContentTypes["tex"] = "application/x-tex"
httpContentTypes["wasm"] = "application/wasm"
httpContentTypes["woff"] = "application/font-woff"
httpContentTypes["woff2"] = "application/font-woff2"
httpContentTypes["eot"] = "application/vnd.ms-fontobject"
httpContentTypes["ttf"] = "application/x-font-ttf"
httpContentTypes["otf"] = "application/x-font-opentype"
httpContentTypes["sfnt"] = "application/font-sfnt"
httpContentTypes["bin"] = "application/octet-stream"
httpContentTypes["csv"] = "text/csv"
httpContentTypes["tsv"] = "text/tab-separated-values"
httpContentTypes["ics"] = "text/calendar"
httpContentTypes["rtx"] = "text/richtext"
httpContentTypes["css"] = "text/css"
httpContentTypes["vtt"] = "text/vtt"
httpContentTypes["mpg"] = "video/mpeg"
httpContentTypes["mpeg"] = "video/mpeg"
httpContentTypes["mp4"] = "video/mp4"
httpContentTypes["m4v"] = "video/x-m4v"
httpContentTypes["webm"] = "video/webm"
httpContentTypes["ogv"] = "video/ogg"
httpContentTypes["mov"] = "video/quicktime"
httpContentTypes["flv"] = "video/x-flv"
httpContentTypes["wmv"] = "video/x-ms-wmv"
httpContentTypes["avi"] = "video/x-msvideo"
httpContentTypes["mp3"] = "audio/mpeg"
httpContentTypes["m4a"] = "audio/x-m4a"
httpContentTypes["wav"] = "audio/wav"
httpContentTypes["ogg"] = "audio/ogg"
httpContentTypes["weba"] = "audio/webm"
httpContentTypes["aac"] = "audio/aac"
httpContentTypes["flac"] = "audio/flac"
httpContentTypes["mid"] = "audio/midi"
httpContentTypes["midi"] = "audio/midi"

-- @brief Returns the value for the given key if exists and returns "txt" if not.
function httpContentTypes.get(key)
    if httpContentTypes[key] ~= nil then
        return httpContentTypes[key]
    else
        return httpContentTypes["txt"]
    end
end