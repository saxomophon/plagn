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
    m_workerThread = jthread(bind(&loop, this));
}

void Plag::loop(std::stop_token stopToken)
{
    while (!stopToken.stop_requested())
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
