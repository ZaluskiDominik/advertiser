//IMPLEMENTATION OF TEMPLATE CLASS SocketHandler

#include <QDebug>

template <class CallbacksClass>
SocketHandler<CallbacksClass>::SocketHandler(const CallbacksClass& _callbacksInst, qintptr socketId, QObject *parent)
    :QObject(parent), callbacksInst(_callbacksInst)
{
    //if couldn't set socket descriptor, exit function
    if ( ! ( isInit = socket.setSocketDescriptor(socketId) ) )
        return;

    //connect socket's signals to slots
    QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    QObject::connect(&socket, SIGNAL(disconnect()), this, SIGNAL(disconnected(this)));
}

template<class CallbacksClass>
void SocketHandler<CallbacksClass>::send(const QByteArray& data)
{
    QByteArray dataBuffer;
    QDataStream stream(&dataBuffer, QIODevice::WriteOnly);

    //append dummy quint32 var to dataBuffer, it will act as container for size of send data
    stream << quint32(0);
    //append rest of data to bufferData from data variable
    dataBuffer.append(data);

    //go to start of the dataBufefr
    stream.device()->seek(0);
    //append to dataBuffer info about send data's size
    stream << static_cast<quint32>(data.size());

    //send prepared dataBuffer
    socket.write(dataBuffer);
}

template <class CallbacksClass>
bool SocketHandler<CallbacksClass>::isInitalized()
{
    return isInit;
}

template<class CallbacksClass>
void SocketHandler<CallbacksClass>::addReadyReadCallback(Requests req, void (CallbacksClass::* callback)(QDataStream &, SocketHandler<CallbacksClass> *))
{
    //if a size of the array with callbacks is too small for request code, then resize it
    if ( readyReadCallbacks.size() <= static_cast<unsigned int>(req) )
        readyReadCallbacks.resize(static_cast<int>(req) + 1);

    //add callback to the array
    readyReadCallbacks[static_cast<int>(req)] = callback;
}

template<class CallbacksClass>
void SocketHandler<CallbacksClass>::wholeMsgComposed(QDataStream& msgStream)
{
    //retrieve request's name
    Requests req;
    msgStream >> req;

    //if request is recognized
    if ( req < readyReadCallbacks.size() )
    {
        //invoke appropriate callback function that will handle set data
        callbacksInst.*readyReadCallbacks[req](msgStream, this);
    }
    else
    {
        //unknown request
        qDebug() << "Bad request!";
    }
}

template <class CallbacksClass>
void SocketHandler<CallbacksClass>::onReadyRead()
{
    //append read bytes to buffer which compose all message's chunks after each ready read occurs
    msgBuffer.append( socket.readAll() );
    //initialize serialization object
    QDataStream msgStream(&msgBuffer, QIODevice::ReadOnly);

    //get info about expected whole message's size
    quint32 msgSize;
    msgStream >> msgSize;

    //if all message's chunks were received
    if (msgSize == msgBuffer.size())
        wholeMsgComposed(msgStream);
}
