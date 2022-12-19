/**
 *-------------------------------------------------------------------------------------------------
 * @file LuaWrapper.hpp
 * @author Fabian Köslin (fabian@koeslin.info)
 * @contributors: 
 * @brief Holds the LuaWrapper Class
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

#ifndef LUAWRAPPER_HPP
#define LUAWRAPPER_HPP

// std includes

// lua includes
#include <lua.hpp>

/**
 * ------------------------------------------------------------------------------------------------
 * @brief The PlagInterface class is an interface for Plags and Kables to work together
 * 
 */
class LuaWrapper
{
public:
    LuaWrapper();
    ~LuaWrapper();
    
private:
    lua_State * m_luaState;
};

#endif // LUAWRAPPER_HPP