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

// own includes
#include "Utilities.hpp"

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

    // add and get a object to the extra storage
    template<class T>
    void addObjectPtr(T * ptr)
    {
        *static_cast<T **>(lua_getextraspace(m_luaState)) = ptr;
    }

    template<class T>
    static T * getObjectPtr(lua_State * L)
    {
        return *static_cast<T **>(lua_getextraspace(L));
    }
    
    template<class T>
    T * getObjectPtr()
    {
        return getObjectPtr<T>(m_luaState);
    }
    
    // create Lua globals
    // static
    static void createTable(lua_State * L, const std::string & name, const std::map<std::string, DataType> & mapValues);
    static void createTable(lua_State * L, const std::string & name, const std::map<std::string, std::string> & mapValues);
    static void createString(lua_State * L, const std::string & name, const std::string & value);
    static void createInteger(lua_State * L, const std::string & name, int value);
    static void createNumber(lua_State * L, const std::string & name, double value);
    static void createFunction(lua_State * L, const std::string & name, lua_CFunction func);
    
    // non static
    void createTable(const std::string & name, const std::map<std::string, DataType> & mapValues);
    void createTable(const std::string & name, const std::map<std::string, std::string> & mapValues);
    void createString(const std::string & name, const std::string & value);
    void createInteger(const std::string & name, int value);
    void createNumber(const std::string & name, double value);
    void createFunction(const std::string & name, lua_CFunction func);
    
    // get Lua globals
    // static
    // index based
    static std::map<std::string, DataType> getTable(lua_State * L, int idx);
    static std::string getString(lua_State * L, int idx);
    static int getInteger(lua_State * L, int idx);
    static double getNumber(lua_State * L, int idx);

    // name based
    static std::map<std::string, DataType> getTable(lua_State * L, const std::string & name);
    static std::string getString(lua_State * L, const std::string & name);
    static int getInteger(lua_State * L, const std::string & name);
    static double getNumber(lua_State * L, const std::string & name);

    // non static
    std::map<std::string, DataType> getTable(const std::string & name);
    std::string getString(const std::string & name);
    int getInteger(const std::string & name);
    double getNumber(const std::string & name);
    
    // execute code
    int executeFile(const std::string & filename);
    int executeString(const std::string & code);
    
private:
    lua_State * m_luaState;
};

#endif // LUAWRAPPER_HPP