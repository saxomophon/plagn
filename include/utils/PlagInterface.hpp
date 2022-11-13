/**
 *-------------------------------------------------------------------------------------------------
 * @file PlagInterface.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors: 
 * @brief Holds the PlagInterface class and the PlagType enum
 * @version 0.1
 * @date 2022-05-20
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

#ifndef PLAGINTERFACE_HPP
#define PLAGINTERFACE_HPP

// std includes
#include <list>
#include <memory>

// own includes
#include "datagrams/Datagram.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief convenience enum, to check between Plag types
 *
 */
enum PlagType : unsigned int
{
    UDP,
    none = UINT_MAX
};

/**
 * ------------------------------------------------------------------------------------------------
 * @brief The PlagInterface class is an interface for Plags and Kables to work together
 * @details The idea goes as follows: For Kables to actually transport datagrams from one Plag to
 * another, the Kable has to know about a Plag's methods of at leat receiving datagrams. However,
 * the source Plag shall be "owner" of the Kable. To avoid forward declarations, this interface
 * class is defined to let Kables know of the methods it needs to place/pull datagrams to/from
 * Plags. Plags will simply implement these abstract methods when inheriting and therefore keep
 * everything compiling, hopefully.
 * 
 */
class PlagInterface
{
public:
    PlagInterface(PlagType type = PlagType::none);

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram) = 0;

    PlagType getType() const;
    
protected:
    std::list<std::shared_ptr<Datagram>> m_outgoingDatagrams;    //!< datagrams to be delivered to Kables very briefly live here.
    std::list<std::shared_ptr<Datagram>> m_incommingDatagrams;   //!< datagrams translated and delivered by Kables very briefly live here.
private:
    PlagType m_type;                //!< type of Plag (for convenience checking)
    bool m_streamableDataAvailable; //!< wheter or not, this can handle streamable data
};

#endif // PLAGINTERFACE_HPP