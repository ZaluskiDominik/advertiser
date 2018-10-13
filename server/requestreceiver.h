#ifndef REQUESTRECEIVER_H
#define REQUESTRECEIVER_H

#include "requests.h"

class SocketHandler;

//class that have to be inherited in order to receive requests/responses
class RequestReceiver
{
    friend class SocketHandler;
public:
    virtual ~RequestReceiver() {}

protected:
    //pure virtual function to implement by every descendant class
    //invoked by socket handler when request/response is redirected to this class descendand
    virtual void onDataReceived(Request req, SocketHandler* sender) = 0;
};

#endif // REQUESTRECEIVER_H
