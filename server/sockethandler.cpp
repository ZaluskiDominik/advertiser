#include "sockethandler.h"
#include <QDebug>

SocketHandler::SocketHandler(qintptr socketId, QObject *parent)
    :QObject(parent)
{
    //if couldn't set socket descriptor, throw exception
    if ( !socket.setSocketDescriptor(socketId) )
        throw QString("Couldn't set socket descriptor!");

    //connect socket's slots
    QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    QObject::connect(&socket, SIGNAL(disconnected()), this, SIGNAL(disconnected(this)));

    //init size of requestsReceivers array
    requestsReceivers.resize(Request::Count);
}

void SocketHandler::send(const Request& request)
{
    QByteArray dataBuffer;
    QDataStream stream(&dataBuffer, QIODevice::WriteOnly);

    //append dummy quint32 var to dataBuffer, it will act as container for size of send data
    stream << quint32(0);
    //append rest of data to bufferData from data variable
    dataBuffer.append(request);

    //go to start of the dataBufefr
    stream.device()->seek(0);
    //append to dataBuffer info about send data's size
    stream << static_cast<quint32>(dataBuffer.size());

    //send prepared dataBuffer
    socket.write(dataBuffer);
}

void SocketHandler::addRequestReceiver(std::vector<Request::RequestName> requestsNames, RequestReceiver &receiver)
{
    //for all requests' names
    for (auto i = requestsNames.begin() ; i != requestsNames.end() ; i++)
    {
        //if request's code is valid
        if ( static_cast<unsigned int>(*i) < requestsReceivers.size() )
        {
            //append request's receiver of given request to list
            requestsReceivers[*i].push_back(&receiver);
        }
    }
}

void SocketHandler::wholeMsgComposed(Request& request)
{
    //if request is recognized
    if ( request.name < requestsReceivers.size() )
    {
        //for all request receivers that listen to request.name code request
        for (auto i = requestsReceivers[request.name].begin() ; i != requestsReceivers[request.name].end() ; i++)
        {
            //redirect request's data to appropriate instance of RequestReceiver class
            (*i)->onDataReceived(request, this);
        }
    }
    else
    {
        //unknown request
        qDebug() << "Bad request!";
    }
}

void SocketHandler::onReadyRead()
{
    //append read bytes to buffer which compose all message's chunks after each ready read occurs
    msgBuffer.append( socket.readAll() );
    //initialize serialization object
    QDataStream msgStream(&msgBuffer, QIODevice::ReadOnly);

    //get info about expected whole message's size
    quint32 msgSize;
    msgStream >> msgSize;

    //if all message's chunks were received
    if ( msgSize == static_cast<quint32>(msgBuffer.size()) )
    {
        //retrieve Request object
        Request req;
        msgStream >> req;
        //proceed data
        wholeMsgComposed(req);

        //clear buffer to next request
        msgBuffer.clear();
    }
}
