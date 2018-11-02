#include "sockethandler.h"

quint32 RequestReceiver::receiversCounter = 0;

RequestReceiver::RequestReceiver()
{
    receiverId = receiversCounter++;
}

RequestReceiver::~RequestReceiver()
{
    for (auto i = registeredSockets.begin() ; i != registeredSockets.end() ; i++)
        (*i)->removeRequestReceiver(*this);
}

quint32 RequestReceiver::getReceiverId()
{
    return receiverId;
}

void RequestReceiver::registerRequestsReceiver(SocketHandler *socketHandler)
{
    //if socket handler isn't already contained in registeredSockets vector
    if ( std::find(registeredSockets.begin(), registeredSockets.end(), socketHandler) == registeredSockets.end() )
        registeredSockets.push_back(socketHandler);
}
