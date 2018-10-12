#ifndef ABSTRACTSERVER_H
#define ABSTRACTSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <vector>
#include "sockethandler.h"
#include "requests.h"

struct User;

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(int port, QObject* parent = nullptr);
    ~TcpServer();

    //try starting server on a given port
    bool startServer();

private slots:
    //function called when user disconnects
    void onUserDisconnected(SocketHandler<TcpServer>* socketHandler);

private:
    int port;

    //array of connected users
    std::vector<User*> users;

    //returns user's id from database
    void getUserId(QString login);

    //registers callbacks to different requests sent to ready read event
    void registerSocketRequests(SocketHandler<TcpServer>& socketHandler);

    User& getUserBySocketHandler(SocketHandler<TcpServer>* socketHandler);

    //new connection has been made
    void incomingConnection(qintptr sockedId);

    //LIST OF REQUESTS HANDLERS(callbacks to ready read event)
    void onTextRequested(QDataStream& data, SocketHandler<TcpServer>* socketHandler);
};

struct User
{
    User(qintptr socketId, const TcpServer& server)
        :socketHandler(server, socketId)
    {
    }

    //this user's socket wrapper
    SocketHandler<TcpServer> socketHandler;
    //user's id will be pulled later from database after he logs in
    int userId = -1;
};

#endif // ABSTRACTSERVER_H
