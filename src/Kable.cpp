/**
 *-------------------------------------------------------------------------------------------------
 * @file Kable.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements the Kable class
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

// std include
#include <iostream>

// self include
#include "Kable.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Kable:: Kable object sets up the member values, of which Plags need to
 * connected in what order
 *
 * @param parent the source Plag, where Datagrams are from
 * @param target the target Plag, where Datagrams need to be translated to
 */
Kable::Kable(PlagInterface & parent, std::shared_ptr<PlagInterface> target) :
    m_parent(parent),
    m_target(target)
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief simple setter function, re-directing the Datagrams of m_parent to a new @p target
 *
 * @details This function is mainly for failed Plags. Shoudl a Plag be unresponsive or hung up, it
 * should be killed. But then this Kable has no valid target anymore. Also the Plag shoud probably
 * be restartet. Hence, to allow a restart of the target Plag - without killing the parent Plag -
 * this method came in place. Please refrain from abusing it, by sticking to that use-case only.
 *
 * @param target the Plag, where the Datagrams should be delivered to instead
 */
bool Kable::assignTarget(std::shared_ptr<PlagInterface> target)
{
    if (target->getType() == m_targetType)
    {
        m_target = target;
        return true;
    }
    return false;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief the transmit function is the public interface for message translation and deliver to
 * the target.
 *
 * @param datagram a Datagram to be translated and delivered
 */
void Kable::transmit(shared_ptr<Datagram> datagram)
{
    shared_ptr<Datagram> translatedDatagram = translate(datagram);
    deliver(translatedDatagram);
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief producing a Datagram the target Plag knows how to handle from the provided
 * @p sourceDatagram . Making use of the config file for the correct translation.
 *
 * @param sourceDatagram the Datagram of the parent
 * @return shared_ptr<Datagram> a Datagram the target Plag will understand
 */
shared_ptr<Datagram> Kable::translate(shared_ptr<Datagram> sourceDatagram)
{
    switch (m_targetType)
    {
    case PlagType::UDP:
        break;
    case PlagType::none:
        //deliberate fall-through
    default:
        cout << "Nothing to do here!" << endl;
        return nullptr;
    }
    return nullptr;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief bringing the Datagram to the target Plag, by placing it there
 * 
 * @param datagram the translated-for-the-target-Plag Datagram
 *
 * @sa Plag::placeDatagram()
 */
void Kable::deliver(shared_ptr<Datagram> datagram)
{
    m_target->placeDatagram(datagram);
}
