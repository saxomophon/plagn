/**
 *-------------------------------------------------------------------------------------------------
 * @file Plag.cpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Implements the Plag class' non-abstract functions
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

// std includes
#include <iostream>
#include <functional>

// self include
#include "Plag.hpp"

using namespace std;

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Construct a new Plag:: Plag object the default constructor for most subclasses simply
 * calls readConfig()
 *
 * @sa Plag::readConfig()
 *
 */
Plag::Plag(const string & name, const uint64_t & id, PlagType type) :
    PlagInterface(type),
    m_name(name),
    m_plagId(id),
    m_workerThread(),
    m_stopToken(false)
{
    readConfig();
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief evaluating and applying the settings file
 * 
 */
void Plag::readConfig()
{
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief Plag::init() is the method to actually set-up everything before the worker is free to
 * start.
 *
 * @details The idea to call the constructor, doing the config readout and the initializitation in
 * seperate methods was taken, to be more modular about it, accepting the cost of a more complex
 * start-up procedure. This may be altered in the future.
 *
 */
void Plag::init()
{
    // to do nothing should suit most cases
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief creating a thread for the main execution to run. calls Plag::loop()
 *
 * @sa Plag::loop()
 *
 */
void Plag::startWorker()
{
    m_workerThread = thread(bind(&Plag::loop, this, this->m_stopToken));
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief accessible value setter, marking the token to stop the main thread (=worker)
 * 
 */
void Plag::stopWork()
{
    m_stopToken = true;
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief loop is the regular main thread's execution envelop. It runs indefinetly, calls loopWork()
 * - an abstract method defined by each subclass - and sleeps for a millisecond afterwards if
 * loopWork() reported idleness by returning false.
 * This indefinite loop can be broken by setting the @p stopToken to true
 *
 * @param stopToken a reference to a boolean, that indicates whether to stop the process (=true) or
 * continue the loop (=false)
 */
void Plag::loop(const bool & stopToken)
{
    while (!stopToken)
    {
        if (!this->loopWork())
        {
            // no more work to do - worker is idle
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    }
}

/**
 * -------------------------------------------------------------------------------------------------
 * @brief placeDatagram is a function to place a Datagram here. Hence this method decodes the
 * datagram
 *
 * @param datagram A Datagram containing data for this Plag to interprete
 */
void Plag::placeDatagram(const shared_ptr<Datagram> datagram)
{
    if (datagram)
    {
        cout << "Hello World!" << endl;
    }
}

/**
 *-------------------------------------------------------------------------------------------------
 * @brief distribute is the method to take a message from the outgoing buffer and provide it to
 * every attached Kable.
 *
 */
void Plag::distribute()
{
    // if outgoing datagrams list is not empty: distribute
    if (m_outgoingDatagrams.begin() != m_outgoingDatagrams.end())
    {
        for (shared_ptr<Kable> kable : m_kables)
        {
            kable->transmit(m_outgoingDatagrams.front());
        }
        m_outgoingDatagrams.pop_front();
    }
}
