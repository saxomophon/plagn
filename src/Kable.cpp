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

// own include
#include "DatagramMqtt.hpp"
#include "DatagramUdp.hpp"
#include "DatagramMap.hpp"
#include "DatagramHttpServer.hpp"

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
Kable::Kable(const boost::property_tree::ptree & propTree, const string & rootName,
             weak_ptr<PlagInterface> parent, weak_ptr<PlagInterface> target) :
    PropertyTreeReader(propTree, rootName),
    m_parent(parent),
    m_target(target),
    m_targetType(none)
{
    if (!target.expired()) m_targetType = target.lock()->getType();
    readConfig();
}

void Kable::readConfig() try
{
    vector<string> keys = getKeys();
    for (const string & key : keys)
    {
        if (key == "sourcePlag" || key == "targetPlag")
        {
            continue;
        }
        else if (key == "gateCondition")
        {
            //TODO: create Gates
            continue;
        }
        else
        {
            m_translationMap.insert_or_assign(key, getParameter<string>(key));
        }
    }
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in Kable::readConfig(): ") + e.what());
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
bool Kable::assignTarget(weak_ptr<PlagInterface> target) try
{
    if (!target.expired() && target.lock()->getType() == m_targetType)
    {
        m_target = target;
        return true;
    }
    else if (m_target.expired() || m_targetType == none)
    {
        m_target = target;
        m_targetType = target.lock()->getType();
    }
    return false;
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in Kable::assignTarget(): ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief the transmit function is the public interface for message translation and deliver to
 * the target.
 *
 * @param datagram a Datagram to be translated and delivered
 */
void Kable::transmit(shared_ptr<Datagram> datagram) try
{
    shared_ptr<Datagram> translatedDatagram = translate(datagram);
    deliver(translatedDatagram);
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in Kable::transmit(): ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief producing a Datagram the target Plag knows how to handle from the provided
 * @p sourceDatagram . Making use of the config file for the correct translation.
 *
 * @param sourceDatagram the Datagram of the parent
 * @return shared_ptr<Datagram> a Datagram the target Plag will understand
 */
shared_ptr<Datagram> Kable::translate(shared_ptr<Datagram> sourceDatagram) try
{
    shared_ptr<Datagram> translatedDatagram = nullptr;
    string sourcePlag = get<string>(sourceDatagram->getData("sourcePlag"));
    switch (m_targetType)
    {
    case PlagType::MQTT:
        translatedDatagram = shared_ptr<DatagramMqtt>(new DatagramMqtt(sourcePlag));
        break;
    case PlagType::UDP:
        translatedDatagram = shared_ptr<DatagramUdp>(new DatagramUdp(sourcePlag));
        break;
    case PlagType::HttpServer:
        translatedDatagram = shared_ptr<DatagramHttpServer>(new DatagramHttpServer(sourcePlag));
        break;
    case PlagType::none:
        //deliberate fall-through
    default:
        cout << "Nothing to do here!" << endl;
        return nullptr;
    }

    // working through translation table
    for (const pair<string, string> & mapEntry : m_translationMap)
    {
        try
        {
            if (mapEntry.first == "sourcePlag"
            || mapEntry.first == "targetPlag"
            || mapEntry.first == "gateCondition") continue;
            translatedDatagram->setData(mapEntry.first,
                                        sourceDatagram->getData(mapEntry.second));
        }
        catch (std::invalid_argument & e)
        {
            cout << "Invalid argument: " << e.what();
            cout << "  -> Key \"" << mapEntry.second << "\" will be ignored!" << endl;
            continue;
        }
    }

    return translatedDatagram;
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in Kable::translate(): ") + e.what());
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief bringing the Datagram to the target Plag, by placing it there
 * 
 * @param datagram the translated-for-the-target-Plag Datagram
 *
 * @sa Plag::placeDatagram()
 */
void Kable::deliver(shared_ptr<Datagram> datagram) try
{
    // as Kable is owned by Plag, Kable has to check whether weak_ptr still points to valid target
    if (!m_target.expired()) m_target.lock()->placeDatagram(datagram);
}
catch (exception & e)
{
    throw std::runtime_error(string("Happened in Kable::deliver(): ") + e.what());
}
