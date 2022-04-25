#ifndef PLAG_HPP
#define PLAG_HPP

// own includes
#include "utils/PlagInterface.hpp"

class Plag : public PlagInterface
{
public:
    Plag();

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram);
};

#endif // PLAG_HPP
