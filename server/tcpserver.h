#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <vector>
#include "../shared/sockethandler.h"
#include "../shared/requests.h"
#include "../shared/requestreceiver.h"
#include "../shared/pricelist.h"
#include "../shared/userdata.h"
#include "dbmanager.h"

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
    //overriden virtual function that receive request and invokes appropriate function handling that requests
    void onDataReceived(Request request, SocketHandler* sender) final;

private:
    int port;

    //array of connected users
    std::vector<User*> users;

    //array of pointers to functions that are invoked on appropriate request from client
    std::vector< void (TcpServer::*)(Request&, SocketHandler*, DBManager& db) > responsesToRequests;

    //list all functions in responsesToRequests array that will handle each request from client
    void initResponsesToRequests();

    //returns instance of User class that owns given socketHandler
    User& getUserBySocketHandler(SocketHandler* socketHandler);

    //new connection has been made
    void incomingConnection(qintptr sockedId);

    //LIST OF REQUESTS HANDLERS(callbacks to ready read event)
    //****************************************************************************

    //client has requested login credentials authentication
    void onLoginAuthRequest(Request& req, SocketHandler* socketHandler, DBManager &db);
    //send response if login was successfull
    void sendLoginAuthResponse(quint32 receiverId, SocketHandler *socketHandler, const UserData &userData, bool isAuth);

    //client wants to get data about given user
    void onGetUserDataRequest(Request& req, SocketHandler* socketHandler, DBManager &db);

    //returns UserData object converted from query records
    UserData convertToUserData(QSqlQuery* query);
    //client wants to change his data, save changes in db
    void onChangeUserDataRequest(Request& req, SocketHandler* socketHandler, DBManager &db);
    //saves changes in user's data in db
    bool saveUserData(const UserData& userData, DBManager &db);

    //client request data of all users excluding admins
    void onGetAllUsersData(Request& req, SocketHandler* socketHandler, DBManager& db);

    //client wants to remove a user from db
    void onDeleteUserRequest(Request& req, SocketHandler* socketHandler, DBManager& db);

    //client request data about active price list
    void onGetActivePriceList(Request& req, SocketHandler* socketHandler, DBManager& db);
    PriceList convertToPriceList(QSqlQuery* query);

    //client requests data about all available price lists
    void onGetAllPriceListsRequest(Request& req, SocketHandler* socketHandler, DBManager& db);

    //client requested the change of currently active price list
    void onChangeActivePriceListRequest(Request& req, SocketHandler* socketHandler, DBManager& db);

    //client requested removal of a price list
    void onRemovePriceListRequest(Request& req, SocketHandler* socketHandler, DBManager& db);

    //client requested savement od changes made to a price list
    void onSavePriceListRequest(Request& req, SocketHandler* socketHandler, DBManager& db);

    //client wants to create a new price list
    void onAddNewPriceListRequest(Request& req, SocketHandler* socketHandler, DBManager& db);

    //client wants to get info about all ads stored in db
    void onGetAdsRequest(Request& req, SocketHandler* socketHandler, DBManager& db);

    void adDataChangeHelper(Request& req, SocketHandler* socketHandler, QSqlQuery* query, bool newAd);
    //user wants to add new ad
    void onAddNewAdRequest(Request& req, SocketHandler* socketHandler, DBManager& db);
    //user wants to modify data of existing ad
    void onModifyAdRequest(Request& req, SocketHandler* socketHandler, DBManager& db);

    //client wants to remove one of his ads
    void onRemoveAdRequest(Request& req, SocketHandler* socketHandler, DBManager& db);
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
