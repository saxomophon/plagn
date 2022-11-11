/**
 *-------------------------------------------------------------------------------------------------
 * @file Utilities.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @brief Implementation of utility functions
 * @version 0.1
 * @date 2022-11-11
 *
 * @copyright Copyright (c) 2022, licensed unter LGPL v2.1
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
#include "Utilities.hpp"

using namespace std;

#ifdef _WIN32
const string plagn::DEFAULT_CONFIG_PATH = "./plagn.conf";
#else
const string plagn::DEFAULT_CONFIG_PATH = "/usr/etc/plagn.conf";
#endif