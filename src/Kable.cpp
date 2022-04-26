// std include
#include <iostream>

// self include
#include "Kable.hpp"

using namespace std;

Kable::Kable(PlagInterface & parent, std::shared_ptr<PlagInterface> target) :
    m_parent(parent),
    m_target(target)
{
}

void Kable::assignTarget(std::shared_ptr<PlagInterface> target)
{
    m_target = target;
}

void Kable::transmit(shared_ptr<Datagram> datagram)
{
    cout << "Helloooo world!" << endl;
}
