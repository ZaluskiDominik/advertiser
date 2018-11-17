#ifndef REQUESTRECEIVER_H
#define REQUESTRECEIVER_H

#include "requests.h"
#include <list>

class SocketHandler;

//class that have to be inherited in order to receive requests/responses
class RequestReceiver
{
    friend class SocketHandler;
public:
    RequestReceiver();
    //removes from socket handlers in registeredSockets list all request that were sent to this RequestReceiver instance
    virtual ~RequestReceiver();

protected:
    //returns receiverId
    quint32 getReceiverId();

    //pure virtual function to implement by every descendant class
    //invoked by socket handler when request/response is redirected to this class descendant
    virtual void onDataReceived(Request req, SocketHandler* sender) = 0;

    //registers request receiver in givern socket handler
    //RequestReceiver instance from that point can receive messages from that socket
    void registerRequestsReceiver(SocketHandler* socketHandler);

private:
    //value incremented when new RequestReceiver is created
    static quint32 receiversCounter;

    //number identifing RequestReceiver instance
    //it's used to dispatching requests to appropriate RequestReceiver instances
    //client that sent request using receiverId will receive response that will be directed to sender RequestReceiver
    quint32 receiverId;

    //list of socket from which RequestReceiver instance gets requests/responses
    std::list<SocketHandler*> registeredSockets;
};

#endif // REQUESTRECEIVER_H
