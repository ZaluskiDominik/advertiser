#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <vector>
#include "sockethandler.h"
#include "requests.h"
#include "requestreceiver.h"
#include "userdata.h"

struct User;

class TcpServer : public QTcpServer, public RequestReceiver
{
    Q_OBJECT
public:
    explicit TcpServer(int port, QObject* parent = nullptr);
    ~TcpServer();

    //try starting server on a given port
    bool startServer();

private slots:
    //function called when user disconnects
    void onUserDisconnected(SocketHandler* socketHandler);

protected:
    //overriden virtual function that receive request and invokes appropriate function handling that request
    void onDataReceived(Request request, SocketHandler* sender) final;

    //register all available requests(listed in requests.h) in socketHandler
    void registerRequestsReceiver(SocketHandler* socketHandler) final;

private:
    int port;

    //array of connected users
    std::vector<User*> users;

    //array of pointers to functions that are invoked on appropriate request from client
    std::vector< void (TcpServer::*)(Request&, SocketHandler*) > responsesToRequests;

    //list all functions in responsesToRequests array that will handle each request from client
    void initResponsesToRequests();

    //returns instance of User class that owns given socketHandler
    User& getUserBySocketHandler(SocketHandler* socketHandler);

    //new connection has been made
    void incomingConnection(qintptr sockedId);

    //LIST OF REQUESTS HANDLERS(callbacks to ready read event)

    //client has requested login credentials authentication
    void onLoginAuthRequest(Request& req, SocketHandler* socketHandler);
    //send response if login was successfull
    void sendLoginAuthResponse(SocketHandler *socketHandler, const UserData &userData, bool isAuth);

    //client wants to change his data, save changes in db
    void onChangeUserDataRequest(Request& req, SocketHandler* socketHandler);
    //saves changes in user's data in db
    bool saveUserData(const UserData& userData, SocketHandler *socketHandler);
};

//container for strorig user's socketHandler and id
struct User
{
    User(qintptr socketId)
        :socketHandler(new SocketHandler(socketId))
    {
    }
    ~User()
    {
        socketHandler->deleteLater();
    }

    //user's socket wrapper
    SocketHandler* socketHandler;
    //user's id will be pulled later from database after he logs in
    int userId = -1;
    //whether this user is an admin
    bool isAdmin = false;
};

#endif
