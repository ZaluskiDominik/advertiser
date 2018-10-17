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
    //removes from socket handlers in registeredSockets list all request that were sent to this RequestReceiver instance
    virtual ~RequestReceiver();

protected:
    //pure virtual function to implement by every descendant class
    //invoked by socket handler when request/response is redirected to this class descendant
    virtual void onDataReceived(Request req, SocketHandler* sender) = 0;

    //lists all requests that instance of descendant of this class will be listening to
    //descendant class must implement this function and call default implemntation derivered from RequestReceiver
    virtual void registerRequestsReceiver(SocketHandler* socketHandler) = 0;

private:
    //list of socket from which RequestReceiver instance gets requests/responses
    std::list<SocketHandler*> registeredSockets;
};

#endif // REQUESTRECEIVER_H
