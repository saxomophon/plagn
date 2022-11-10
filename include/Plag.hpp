#ifndef PLAG_HPP
#define PLAG_HPP

// std includes
#include <exception>
#include <thread>

// own includes
#include "utils/PlagInterface.hpp"

class Plag : public PlagInterface
{
public:
    Plag();

    void readConfig();

    virtual void init();

    virtual void startWorker();

    virtual void stopWork();

    virtual void loop(const bool & stopToken);

    virtual void loopWork() = 0;

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram);

protected:
    std::thread m_workerThread;
    bool m_stopToken; //!< central token to stop worker thread
};

#endif // PLAG_HPP
