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
    explicit SocketHandler(qintptr socketId, QObject *parent = nullptr);

    //sends data through the socket
    void send(const Request& request);

    //adds request receiver to requestReceivers array
    //instance of descendant from RequestReceiver will receive data from listed requests
    void addRequestReceiver(std::vector<Request::RequestName> requestsNames, RequestReceiver &receiver);

protected:
    QTcpSocket socket;
    //invokes appropriate callback depending from what request's name is
    void wholeMsgComposed(Request &request);

private:
    //array of bytes composing chunks of sent message
    QByteArray msgBuffer;

    //for each request's code there is appropriate callback function that will be invoked
    //request code is equal to array's index
    std::vector< std::list<RequestReceiver*> > requestsReceivers;

private slots:
    //function invoked whenever data is received
    void onReadyRead();

signals:
    //socket disconnected
    void disconnected(SocketHandler*);
};

#endif // SOCKETHANDLER_H
