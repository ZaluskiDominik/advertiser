#include "tcpserver.h"
#include <QtDebug>

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
        &TcpServer::onTextRequested
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

    //receive thsi user's requests
    registerRequestsReceiver(&user->socketHandler);
    //connect user's socket disconnection signal to slot handling that event
    QObject::connect(&user->socketHandler, SIGNAL(disconnected(SocketHandler*)), this, SLOT(onUserDisconnected()));
    //add user to array
    users.push_back(user);
}

void TcpServer::registerRequestsReceiver(SocketHandler *socketHandler)
{
    //receive all request listed in requests.h
    std::vector<Request::RequestName> requestsNames;
    for (int i = 0 ; i < Request::Count ; i++)
        requestsNames.push_back( static_cast<Request::RequestName>(i) );

    //register that requests in socketHandler
    socketHandler->addRequestReceiver(requestsNames, *this);
}

void TcpServer::getUserId(QString login)
{
    Q_UNUSED(login);
}

User &TcpServer::getUserBySocketHandler(SocketHandler *socketHandler)
{
    unsigned int i;
    for (i = 0 ; &users[i]->socketHandler != socketHandler ; i++);
    return *users[i];
}

void TcpServer::onTextRequested(Request &data, SocketHandler *socketHandler)
{
    qDebug() << "Text request: " << data.data << "\nId: " << getUserBySocketHandler(socketHandler).userId;
}
