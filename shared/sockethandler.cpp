#include "sockethandler.h"
#include <QDebug>

SocketHandler::SocketHandler(QObject *parent)
    :QObject(parent)
{
    serverSide = false;
}

SocketHandler::SocketHandler(qintptr socketId, QObject *parent)
    :QObject(parent)
{
    serverSide = true;

    //if couldn't set socket descriptor, throw exception
    if ( !socket.setSocketDescriptor(socketId) )
        throw QString("Couldn't set socket descriptor!");

    init();
}

SocketHandler::~SocketHandler()
{
    for (auto i = requestsReceivers.begin() ; i != requestsReceivers.end() ; i++)
            (*i)->registeredSockets.remove(this);
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

void SocketHandler::addRequestReceiver(RequestReceiver &receiver)
{
    //if that receiver wasn't already added
    if ( std::find(requestsReceivers.begin(), requestsReceivers.end(), &receiver) == requestsReceivers.end() )
        requestsReceivers.push_back(&receiver);
}

void SocketHandler::removeRequestReceiver(RequestReceiver &receiver)
{
    requestsReceivers.remove(&receiver);
}

RequestReceiver *SocketHandler::getReceiverById(const quint32 &receiverId)
{
    std::list<RequestReceiver*>::iterator i;
    for (i = requestsReceivers.begin() ; i != requestsReceivers.end() && (*i)->getReceiverId() != receiverId ; i++);
    return ( i != requestsReceivers.end() ) ? *i : nullptr;
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
    if ( request.name < Request::Count )
    {
        //if it's socket handler created by server, redirect request to tcpServer RequestReceiver
        if (serverSide)
            requestsReceivers.front()->onDataReceived(request, this);
        else
        {
            //get RequestReceiver instance that sent that request
            RequestReceiver* receiver = getReceiverById(request.receiverId);
            //if receiver was found
            if (receiver != nullptr)
            {
                //redirect request's data to that sender
                receiver->onDataReceived(request, this);
            }
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
    while (msgBuffer.size())
    {
        //initialize serialization object
        QDataStream msgStream(&msgBuffer, QIODevice::ReadOnly);

        //get info about expected whole message's size
        quint32 msgSize;
        msgStream >> msgSize;

        //if size of the message isn't available, exit
        if ( msgStream.status() == QDataStream::ReadCorruptData )
            return;

        //if all request's chunks were sent
        if ( static_cast<quint32>(msgBuffer.size()) >= msgSize )
        {
            //read bytes of one request
            int streamPos = static_cast<int>(msgStream.device()->pos());
            QByteArray reqBytes = msgBuffer.mid(streamPos, static_cast<int>(msgSize) - streamPos);
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
