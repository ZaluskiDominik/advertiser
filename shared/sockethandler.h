#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QTcpSocket>
#include <vector>
#include <list>
#include <QString>
#include <QByteArray>
#include <QDataStream>
#include "requests.h"
#include "requestreceiver.h"

class SocketHandler : public QObject
{
    Q_OBJECT
public:
    //client socket constuctor
    explicit SocketHandler(QObject *parent = nullptr);
    //server socket constructor
    explicit SocketHandler(qintptr socketId, QObject *parent = nullptr);
    //removes all pointers to RequestReceiver instaces that were registered to receive requests from this socket handler
    ~SocketHandler();

    //connects to host, returns true if connection was made
    bool connectToHost(QString host, quint16 port, int msWaitTime);

    //whether there is a connection to host
    bool isConnected();

    //sends data through the socket
    void send(const Request& request);

    //adds request receiver to requestReceivers array
    //from that point RequestReceiver descendant can receive messages from this socketHandler
    void addRequestReceiver(RequestReceiver &receiver);

    //removes RequestReceiver instance from requestsReceivers array of lists
    //that instance won't get futher any request from this socket handler
    void removeRequestReceiver(RequestReceiver &receiver);

private:
    QTcpSocket socket;

    //whether there is a connection to host
    bool connected = false;

    //whether server constructed that socket handler
    bool serverSide;

    //array of bytes composing chunks of sent message
    QByteArray msgBuffer;

    //list of registered requests receivers
    std::list<RequestReceiver*> requestsReceivers;

    //retuns pointer to RequestReceiver instance found by receiverId, or nullptr if pointer was not found
    RequestReceiver* getReceiverById(const quint32& receiverId);

    //composes request from sent chunks
    void composeRequest(QByteArray& bytes);

    //invokes appropriate callback depending from what request's name is
    void wholeMsgComposed(Request &request);

    //connets to socket signals
    void init();

private slots:
    //function invoked whenever data is received
    void onReadyRead();
    void onDisconnected();

signals:
    //socket disconnected
    void disconnected(SocketHandler*);
};

#endif // SOCKETHANDLER_H
