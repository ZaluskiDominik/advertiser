#include "sockethandler.h"
#include <QDebug>

SocketHandler::SocketHandler(QObject *parent)
    :QObject(parent)
{
    //init size of requestsReceivers array
    requestsReceivers.resize(Request::Count);
}

SocketHandler::SocketHandler(qintptr socketId, QObject *parent)
    :QObject(parent)
{
    //init size of requestsReceivers array
    requestsReceivers.resize(Request::Count);

    //if couldn't set socket descriptor, throw exception
    if ( !socket.setSocketDescriptor(socketId) )
        throw QString("Couldn't set socket descriptor!");

    init();
}

SocketHandler::~SocketHandler()
{
    for (auto i = requestsReceivers.begin() ; i != requestsReceivers.end() ; i++)
    {
        for (auto j = i->begin() ; j != i->end() ; j++)
            (*j)->registeredSockets.remove(this);
    }
}

bool SocketHandler::connectToHost(QString host, quint16 port, int msWaitTime)
{
    socket.connectToHost(host, port);
    bool connMade = socket.waitForConnected(msWaitTime);

    //if connection was made init socket
    if (connMade)
        init();

    return connMade;
}

bool SocketHandler::isConnected()
{
    return connected;
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

void SocketHandler::removeRequestReceiver(RequestReceiver &receiver)
{
    for (auto i = requestsReceivers.begin() ; i != requestsReceivers.end() ; i++)
        i->remove(&receiver);
}

void SocketHandler::composeRequest(QByteArray &bytes)
{
    QDataStream stream(&bytes, QIODevice::ReadOnly);
    //retrieve Request object
    Request req;
    stream >> req;
    //proceed request
    wholeMsgComposed(req);
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

void SocketHandler::init()
{
    connected = true;

    //connect socket's slots
    QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    QObject::connect(&socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void SocketHandler::onReadyRead()
{
    //append read bytes to buffer which compose all message's chunks after each ready read occurs
    msgBuffer.append( socket.readAll() );
    //read each request
    //end loop when buffer is empty all there isn't enough bytes to read request's size
    while ( msgBuffer.size() >= 4 )
    {
        //initialize serialization object
        QDataStream msgStream(&msgBuffer, QIODevice::ReadOnly);

        //get info about expected whole message's size
        quint32 msgSize;
        msgStream >> msgSize;

        //if size couldn't be read, dont't react to request
        if ( msgStream.status() == QDataStream::ReadCorruptData )
        {
            msgBuffer.clear();
            return;
        }

        //if all request's chunks were sent
        if ( static_cast<quint32>(msgBuffer.size()) >= msgSize )
        {
            //read bytes of one request
            QByteArray reqBytes = msgBuffer.mid(4, static_cast<int>(msgSize) - 4);
            //compose request object from those bytes and remove them from msgBuffer
            composeRequest(reqBytes);
            msgBuffer.remove(0, static_cast<int>(msgSize));
        }
        else
            break;
    }
}

void SocketHandler::onDisconnected()
{
    connected = false;
    emit disconnected(this);
}
