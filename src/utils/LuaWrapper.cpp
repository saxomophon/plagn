/**
 *-------------------------------------------------------------------------------------------------
 * @file LuaWrapper.cpp
 * @author Fabian Köslin (fabian@koeslin.info)
 * @contributors:
 * @brief Implements the LuaWrapper class
 * @version 0.1
 * @date 2022-12-19
 *
 * @copyright LGPL v2.1
 *
 * Targets of chosen license for:
 *      Users    : Please be so kind as to indicate your usage of this library by linking to the project
 *                 page, currently being: https://github.com/saxomophon/plagn
 *      Devs     : Your improvements to the code, should be available publicly under the same license.
 *                 That way, anyone will benefit from it.
 *      Corporate: Even you are either a User or a Developer. No charge will apply, no guarantee or
 *                 warranty will be given.
 *
 */

// self include
#include "LuaWrapper.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new LuaWrapper object
 * 
 */
LuaWrapper::LuaWrapper()
{
    m_luaState = luaL_newstate();
    luaL_openlibs(m_luaState);  
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Destruct a new LuaWrapper object
 *
 */
LuaWrapper::~LuaWrapper()
{
    lua_close(m_luaState);
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief create a new Lua table (DataTypes map and vector not supported, yet)
 *
 */
void LuaWrapper::createTable(lua_State * L, const std::map<std::string, DataType> & mapValues)
{
    lua_createtable(L, 0, 4);
    for (auto valuePair : mapValues)
    {
        if (const int * pvar = get_if<int>(&valuePair.second))
        {
            // value is int!
            lua_pushstring(L, valuePair.first.c_str());
            lua_pushinteger(L, *pvar);
            lua_settable(L, -3);
        }
        else if (const unsigned int * pvar = get_if<unsigned int>(&valuePair.second))
        {
            // value is uint!
            lua_pushstring(L, valuePair.first.c_str());
            lua_pushinteger(L, *pvar);
            lua_settable(L, -3);
        }
        else if (const int64_t * pvar = get_if<int64_t>(&valuePair.second))
        {
            // value is int64_t!
            lua_pushstring(L, valuePair.first.c_str());
            lua_pushinteger(L, *pvar);
            lua_settable(L, -3);
        }
        else if (const uint64_t * pvar = get_if<uint64_t>(&valuePair.second))
        {
            // value is uint64_t!
            lua_pushstring(L, valuePair.first.c_str());
            lua_pushinteger(L, *pvar);
            lua_settable(L, -3);
        }
        else if (const double * pvar = get_if<double>(&valuePair.second))
        {
            // value is double!
            lua_pushstring(L, valuePair.first.c_str());
            lua_pushnumber(L, *pvar);
            lua_settable(L, -3);
        }
        else if (const string * pvar = get_if<string>(&valuePair.second))
        {
            // value is string!
            lua_pushstring(L, valuePair.first.c_str());
            lua_pushstring(L, pvar->c_str());
            lua_settable(L, -3);
        }
    }
}

void LuaWrapper::createTable(lua_State * L, const std::map<std::string, std::string> & mapValues)
{
    lua_createtable(L, 0, 4);
    for (auto valuePair : mapValues)
    {
        lua_pushstring(L, valuePair.first.c_str());
        lua_pushstring(L, valuePair.second.c_str());
        lua_settable(L, -3);
    }
}

void LuaWrapper::createString(lua_State * L, const std::string & value)
{
    lua_pushstring(L, value.c_str());
}

void LuaWrapper::createInteger(lua_State * L, int value)
{
    lua_pushinteger(L, value);
}

void LuaWrapper::createNumber(lua_State * L, double value)
{
    lua_pushnumber(L, value);
}

void LuaWrapper::createFunction(lua_State * L, lua_CFunction func)
{
    lua_pushcfunction(L, func);
}

void LuaWrapper::createTable(lua_State * L, const std::string & name, const std::map<std::string, DataType> & mapValues)
{
    createTable(L, mapValues);
    lua_setglobal(L, name.c_str());
}

void LuaWrapper::createTable(lua_State * L, const std::string & name, const std::map<std::string, std::string> & mapValues)
{
    createTable(L, mapValues);
    lua_setglobal(L, name.c_str());
}

void LuaWrapper::createString(lua_State * L, const std::string & name, const std::string & value)
{
    createString(L, value);
    lua_setglobal(L, name.c_str());
}

void LuaWrapper::createInteger(lua_State * L, const std::string & name, int value)
{
    createInteger(L, value);
    lua_setglobal(L, name.c_str());
}

void LuaWrapper::createNumber(lua_State * L, const std::string & name, double value)
{
    createNumber(L, value);
    lua_setglobal(L, name.c_str());
}

void LuaWrapper::createFunction(lua_State * L, const std::string & name, lua_CFunction func)
{
    createFunction(L, func);
    lua_setglobal(L, name.c_str());
}

// non static
void LuaWrapper::createTable(const std::string & name, const std::map<std::string, DataType> & mapValues)
{
    createTable(m_luaState, name, mapValues);
}

void LuaWrapper::createTable(const std::string & name, const std::map<std::string, std::string> & mapValues)
{
    createTable(m_luaState, name, mapValues);
}

void LuaWrapper::createString(const std::string & name, const std::string & value)
{
    createString(m_luaState, name, value);
}

void LuaWrapper::createInteger(const std::string & name, int value)
{
    createInteger(m_luaState, name, value);
}

void LuaWrapper::createNumber(const std::string & name, double value)
{
    createNumber(m_luaState, name, value);
}

void LuaWrapper::createFunction(const std::string & name, lua_CFunction func)
{
    createFunction(m_luaState, name, func);
}

// get Lua globals
// static
map<string, DataType> LuaWrapper::getTable(lua_State * L, int idx)
{
    map<string, DataType> retMap;
    
    if (lua_istable(L, idx))
    {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            if (lua_isinteger(L, -1))
            {
                auto key = lua_tostring(L, -2);
                auto val = static_cast<int>(lua_tointeger(L, -1));
                retMap.insert(pair<string, DataType>(key, val));
            }
            else if (lua_isnumber(L, -1))
            {
                auto key = lua_tostring(L, -2);
                auto val = static_cast<double>(lua_tonumber(L, -1));
                retMap.insert(pair<string, DataType>(key, val));
            }
            else if (lua_isstring(L, -1))
            {
                auto key = lua_tostring(L, -2);
                auto val = lua_tostring(L, -1);
                retMap.insert(pair<string, DataType>(key, val));
            }
            lua_pop(L, 1);
        }
    }
    
    return retMap;
}

map<string, string> LuaWrapper::getTableS(lua_State * L, int idx)
{
    map<string, string> retMap;
    
    if (lua_istable(L, idx))
    {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            if (lua_isstring(L, -1))
            {
                auto key = lua_tostring(L, -2);
                auto val = lua_tostring(L, -1);
                retMap.insert(pair<string, string>(key, val));
            }
            lua_pop(L, 1);
        }
    }
    
    return retMap;
}

std::string LuaWrapper::getString(lua_State * L, int idx)
{
    if (lua_isstring(L, idx))
    {
        return lua_tostring(L, idx);
    }
    return ""; // TODO: Error handling
}

int LuaWrapper::getInteger(lua_State * L, int idx)
{
    if (lua_isinteger(L, idx))
    {
        return lua_tointeger(L, idx);
    }
    return INT_MIN; // TODO: Error handling
}

double LuaWrapper::getNumber(lua_State * L, int idx)
{
    if (lua_isnumber(L, idx))
    {
        return lua_tonumber(L, idx);
    }
    return INT_MIN; // TODO: Error handling
}

std::map<std::string, DataType> LuaWrapper::getTable(lua_State * L, const std::string & name)
{
    lua_getglobal(L, name.c_str());
    return getTable(L, -1);
}

std::map<std::string, string> LuaWrapper::getTableS(lua_State * L, const std::string & name)
{
    lua_getglobal(L, name.c_str());
    return getTableS(L, -1);
}

std::string LuaWrapper::getString(lua_State * L, const std::string & name)
{
    lua_getglobal(L, name.c_str());
    return getString(L, -1);
}

int LuaWrapper::getInteger(lua_State * L, const std::string & name)
{
    lua_getglobal(L, name.c_str());
    return getInteger(L, -1);
}

double LuaWrapper::getNumber(lua_State * L, const std::string & name)
{
    lua_getglobal(L, name.c_str());
    return getNumber(L, -1);
}

// non static
std::map<std::string, DataType> LuaWrapper::getTable(const std::string & name)
{
    return getTable(m_luaState, name);
}

std::map<std::string, string> LuaWrapper::getTableS(const std::string & name)
{
    return getTableS(m_luaState, name);
}

std::string LuaWrapper::getString(const std::string & name)
{
    return getString(m_luaState, name);
}

int LuaWrapper::getInteger(const std::string & name)
{
    return getInteger(m_luaState, name);
}

double LuaWrapper::getNumber(const std::string & name)
{
    return getNumber(m_luaState, name);
}

// execute code
int LuaWrapper::executeFile(const std::string & filename)
{
    return luaL_dofile(m_luaState, filename.c_str());
}

int LuaWrapper::executeString(const std::string & code)
{
    return luaL_dostring(m_luaState, code.c_str());
}
