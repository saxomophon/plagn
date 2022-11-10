// std includes
#include <iostream>

// self include
#include "Plag.hpp"

using namespace std;

Plag::Plag()
{
    readConfig();
}

void Plag::readConfig()
{
}

void Plag::init()
{
    // to do nothing should suit most cases
}

void Plag::startWorker()
{
    m_workerThread = std::thread(bind(&Plag::loop, this, this->m_stopToken));
}

void Plag::stopWork()
{
    m_stopToken = true;
}

void Plag::loop(const bool & stopToken)
{
    while (!stopToken)
    {
        this->loopWork();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Plag::placeDatagram(const std::shared_ptr<Datagram> datagram)
{
    if (datagram)
    {
        cout << "Hello World!" << endl;
    }
}
