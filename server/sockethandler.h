#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QTcpSocket>
#include <vector>
#include <QString>
#include <QByteArray>
#include <QDataStream>
#include "requests.h"

template <class CallbacksClass>
class SocketHandler : public QObject
{
public:
    explicit SocketHandler(const CallbacksClass& _callbacksInst, qintptr socketId, QObject *parent = nullptr);

    //sends data through the socket
    virtual void send(const QByteArray& data);

    //whether socket was succesfully created from socket descriptor
    bool isInitalized();

    //registers a callback function for given request's name
    void addReadyReadCallback(Requests req, void (CallbacksClass::* callback)(QDataStream&, SocketHandler<CallbacksClass>*));

protected:
    QTcpSocket socket;

    //invokes appropriate callback depending from what request's name is
    void wholeMsgComposed(QDataStream &msgStream);

private:
    //whether socket was succesfully created from socket descriptor
    bool isInit;
    //array of bytes composing chunks of sent message
    QByteArray msgBuffer;
    //instance of class that contains read read callback functions
    const CallbacksClass& callbacksInst;

    //array with callback function's pointers
    //for each request's code there is appropriate callback function that will be invoked
    //request code is equal to array's index
    std::vector< void (CallbacksClass::*)(QDataStream&, SocketHandler<CallbacksClass>*) > readyReadCallbacks;

private slots:
    //function invoked whenever data is received
    void onReadyRead();

signals:
    void disconnected(SocketHandler<CallbacksClass>*);
};

#include "sockethandler.tpp"

#endif // SOCKETHANDLER_H
