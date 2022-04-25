// std includes
#include <iostream>

// self include
#include "../include/Plag.hpp"

using namespace std;

Plag::Plag()
{

}

void Plag::placeDatagram(const std::shared_ptr<Datagram> datagram)
{
    if (datagram)
    {
        cout << "Hello World!" << endl;
    }
}
