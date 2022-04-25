#ifndef PLAGINTERFACE_HPP
#define PLAGINTERFACE_HPP

// std includes
#include <memory>

// own includes
#include "datagrams/Datagram.hpp"

/**
 * ------------------------------------------------------------------------------------------------
 * @brief The PlagInterface class is an interface for Plags and Kables to work together
 * @details The idea goes as follows: For Kables to actually transport datagrams from one Plag to
 * another, the Kable has to know about a Plag's methods of at leat receiving datagrams. However,
 * the source Plag shall be "owner" of the Kable. To avoid forward declarations, this interface
 * class is defined to let Kables know of the methods it needs to place/pull datagrams to/from
 * Plags. Plags will simply implement these abstract methods when inheriting and therefore keep
 * everything compiling, hopefully.
 * 
 */
class PlagInterface
{
public:
    PlagInterface();

    virtual void placeDatagram(const std::shared_ptr<Datagram> datagram) = 0;
    //std::shared_ptr<Datagram> pullDatagram();
private:
    bool m_streamableDataAvailable; //!< wheter or not, this can handle streamable data
};

#endif // PLAGINTERFACE_HPP