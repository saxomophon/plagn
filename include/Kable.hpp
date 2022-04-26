#ifndef KABLE_HPP
#define KABLE_HPP

#include "utils/PlagInterface.hpp"

class Kable
{
public:
    Kable(PlagInterface & parent, std::shared_ptr<PlagInterface> target = nullptr);

    void assignTarget(std::shared_ptr<PlagInterface> parent);

    void transmit(std::shared_ptr<Datagram> datagram);

private:
    PlagInterface & m_parent;                   //!< the source Plag (where Datagrams originate)
    std::shared_ptr<PlagInterface> m_target;    //!< the target Plag (where Datagrams are meant to be delivered to)
};

#endif // KABLE_HPP
