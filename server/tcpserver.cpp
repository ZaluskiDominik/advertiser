#include "tcpserver.h"
#include <QtDebug>
#include "dbmanager.h"

TcpServer::TcpServer(int _port, QObject* parent)
    :QTcpServer(parent)
{
    port = _port;
    initResponsesToRequests();
}

TcpServer::~TcpServer()
{
    //remove all users
    for (auto i = users.begin() ; i != users.end() ; i++)
        delete *i;
}

bool TcpServer::startServer()
{
    if ( listen(QHostAddress::Any, quint16(port)) )
    {
        //success
        qDebug() << "Started server!";
        return true;
    }
    else
    {
        //errors occured
        qDebug() << "Could not start a server!";
        return false;
    }
}

void TcpServer::onUserDisconnected(SocketHandler *socketHandler)
{
    User* user = &getUserBySocketHandler(socketHandler);
    //delete user from memory
    delete user;
    //remove user from users array
    auto userIterator = std::find(users.begin(), users.end(), user);
    users.erase(userIterator, userIterator + 1);
}

void TcpServer::onDataReceived(Request request, SocketHandler *sender)
{
    (this->*responsesToRequests[request.name])(request, sender);
}

void TcpServer::initResponsesToRequests()
{
    responsesToRequests = std::vector< void (TcpServer::*)(Request&, SocketHandler*) >({
        &TcpServer::onLoginAuthRequest
    });
}

void TcpServer::incomingConnection(qintptr sockedId)
{
    qDebug() << "New connection";
    //add a new user
    User* user;
    try
    {
        user = new User(sockedId);
    }
    catch(QString& e)
    {
        //error occured, exit rejecting connetion
        qDebug() << e;
        return;
    }

    //receive this user's requests
    registerRequestsReceiver(user->socketHandler);
    //connect user's socket disconnection signal to slot handling that event
    QObject::connect(user->socketHandler, SIGNAL(disconnected(SocketHandler*)), this, SLOT(onUserDisconnected(SocketHandler*)));
    //add user to array
    users.push_back(user);
}

void TcpServer::registerRequestsReceiver(SocketHandler *socketHandler)
{
    //call parent's implementation of this function
    RequestReceiver::registerRequestsReceiver(socketHandler);

    //receive all request listed in requests.h
    std::vector<Request::RequestName> requestsNames;
    for (int i = 0 ; i < Request::Count ; i++)
        requestsNames.push_back( static_cast<Request::RequestName>(i) );

    //register that requests in socketHandler
    socketHandler->addRequestReceiver(requestsNames, *this);
}

User &TcpServer::getUserBySocketHandler(SocketHandler *socketHandler)
{
    unsigned int i;
    for (i = 0 ; users[i]->socketHandler != socketHandler ; i++);
    return *users[i];
}

//RESPONSE TO REQUESTS

void TcpServer::onLoginAuthRequest(Request &req, SocketHandler *socketHandler)
{
    User& user = getUserBySocketHandler(socketHandler);

    //extract login credetials
    QString login, password;
    QDataStream in(&req.data, QIODevice::ReadOnly);
    in >> login >> password;

    //check in database if user with that credentials exists
    DBManager db;
    QSqlQuery* query = db.prepare(QString("SELECT * FROM admins AS a, users AS u LEFT JOIN ") +
    "users ON a.user_id = u.id WHERE u.login = ? AND u.password = ?");
    query->addBindValue(login);
    query->addBindValue(password);
    query->exec();

    bool isAuth = false;
    UserData userData;
    //if no errors occured and number of row is 1 the login was successfull
    if ( query->isActive() && db.rowCount(query) == 1 )
    {
        //authorization passed
        isAuth = true;
        query->first();
        //create container for user data which will be send to client
        userData = UserData(query);
        user.isAdmin = userData.isAdmin;
    }

    //send response to client
    sendLoginAuthResponse(socketHandler, userData, isAuth);
}

void TcpServer::sendLoginAuthResponse(SocketHandler* socketHandler, UserData& userData, bool isAuth)
{
    QByteArray resp;
    QDataStream out(&resp, QIODevice::WriteOnly);

    //whether login credentials are valid
    out << isAuth;
    //container for user data
    out << userData;

    socketHandler->send(Request(Request::LOGIN_AUTH, resp));
}
