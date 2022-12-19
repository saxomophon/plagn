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
