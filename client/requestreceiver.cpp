#include "sockethandler.h"

RequestReceiver::~RequestReceiver()
{
    for (auto i = registeredSockets.begin() ; i != registeredSockets.end() ; i++)
        (*i)->removeRequestReceiver(*this);
}

void RequestReceiver::registerRequestsReceiver(SocketHandler *socketHandler)
{
        registeredSockets.push_back(socketHandler);
}
