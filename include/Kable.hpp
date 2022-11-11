/**
 *-------------------------------------------------------------------------------------------------
 * @file Kable.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @brief Holds the Kable class
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

#ifndef KABLE_HPP
#define KABLE_HPP

// own includes
#include "utils/PlagInterface.hpp"

class Kable
{
public:
    Kable(PlagInterface & parent, std::shared_ptr<PlagInterface> target = nullptr);

    bool assignTarget(std::shared_ptr<PlagInterface> parent);

    void transmit(std::shared_ptr<Datagram> datagram);

    std::shared_ptr<Datagram> translate(std::shared_ptr<Datagram> sourceDatagram);

    void deliver(std::shared_ptr<Datagram> datagram);

private:
    PlagInterface & m_parent;                               //!< the source Plag (where Datagrams originate)
    std::shared_ptr<PlagInterface> m_target;                //!< the target Plag (where Datagrams are meant to be delivered to)
    PlagType m_targetType;                                  //!< the type of the target Plag, to ensure, replacements fit
};

#endif // KABLE_HPP
