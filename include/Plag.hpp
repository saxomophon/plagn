/**
 *-------------------------------------------------------------------------------------------------
 * @file Plag.hpp
 * @author Gerrit Erichsen (saxomophon@gmx.de)
 * @contributors:
 * @brief Holds the Kable class
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

#ifndef PLAG_HPP
#define PLAG_HPP

// std includes
#include <exception>
#include <thread>
#include <vector>

// own includes
#include "utils/PlagInterface.hpp"
#include "Kable.hpp"

/**
 *-------------------------------------------------------------------------------------------------
 * @brief The base class for the operations between protocol end-points and this program.
 *
 * @details Have you ever seen an adapter? Or a dongle for that part? Because the naming is somewhat
 * based on those. The adapter or dongle has one plug on one end, corresponding to a port type,
 * and a different plug on the other end, corresponding to a port of a different type. In between
 * there is a cable connecting the two plugs and making sure, that the pins of one end correspond
 * to the appropriate pins of the other end.
 * Now imagine you're a bad at spelling and want to adapt that idea to protocols. Bam! You've got
 * Plags and Kables.
 * The Plag class hence, does the "port"-adaption. Whatever the protocol and the end-point you want
 * to connect to another end-point, the Plag has to connect to that and understand the data, as well
 * as provide data in the given format. It has to translate data from and end-point into a
 * user-understandable, meaning a Plag'n internal, format. And vice versa, generate data the
 * end-point can interprete from Datagrams of the internal format.
 * Kables are left with the internal translation part and their description can be found there.
 *
 * @sa Kable::Kable()
 *
 */
class Plag : public PlagInterface
{
public:
    Plag(const std::string & name, const uint64_t & id, PlagType type = PlagType::none);

    void readConfig();

    virtual void init();

    virtual void startWorker();

    virtual void stopWork();

    virtual void loop(const bool & stopToken);

    virtual bool loopWork() = 0;

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram);

private:
    void distribute();

protected:
    std::string m_name;         //!< name (descriptive, non-unique if the user desires)
    uint64_t m_plagId;          //!< id of the Plag (unique identifier)
    std::thread m_workerThread; //!< thread to run the communications department (=main thread)
    bool m_stopToken;           //!< central token to stop worker thread

private:
    std::vector<std::shared_ptr<Kable>> m_kables;   //!< all Kables connected to this Plag
};

#endif // PLAG_HPP
