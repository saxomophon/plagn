#ifndef KABLE_HPP
#define KABLE_HPP

#include "utils/PlagInterface.hpp"

class Kable
{
public:
    Kable(PlagInterface & parent);
    Kable(PlagInterface & parent, PlagInterface & target);

    void translate(std::shared_ptr<Datagram> datagram);

private:
    PlagInterface & m_parent;   //!< the source Plag (where Datagrams originate)
    PlagInterface & m_target;   //!< the target Plag (where Datagrams are meant to be delivered to)
};

#endif // KABLE_HPP