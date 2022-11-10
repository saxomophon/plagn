// std includes
#include <iostream>

// self include
#include "Plag.hpp"

using namespace std;

Plag::Plag()
{
    readConfig();
}

void Plag::init()
{
    // to do nothing should suit most cases
}

void Plag::startWorker()
{
    m_workerThread = std::thread(bind(&Plag::loop, this->m_stopToken));
}

void Plag::loop(const bool & stopToken)
{
    while (!stopToken)
    {
        this->loopWork();
    }
}

void Plag::placeDatagram(const std::shared_ptr<Datagram> datagram)
{
    if (datagram)
    {
        cout << "Hello World!" << endl;
    }
}
