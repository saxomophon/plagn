/**
 *-------------------------------------------------------------------------------------------------
 * @file PlagInterface.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @brief Implements the PlagInterface class' non-abstract functions
 * @version 0.1
 * @date 2022-05-20
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
#include "PlagInterface.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Plag Interface:: Plag Interface object
 * 
 * @param type type of Plag
 */
PlagInterface::PlagInterface(PlagType type) :
    m_type(type)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple getter
 * 
 * @return PlagType 
 */
PlagType PlagInterface::getType() const
{
    return m_type;
}
