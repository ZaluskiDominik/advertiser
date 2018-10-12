#include "tcpserver.h"
#include <QtDebug>
#include <QApplication>

TcpServer::TcpServer(int _port, QObject* parent)
    :QTcpServer(parent)
{
    port = _port;
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

void TcpServer::onUserDisconnected(SocketHandler<TcpServer> *socketHandler)
{
    User* user = &getUserBySocketHandler(socketHandler);
    //delete user from memory
    delete user;
    //remove user from users array
    auto userIterator = std::find(users.begin(), users.end(), user);
    users.erase(userIterator, userIterator + 1);
}

void TcpServer::incomingConnection(qintptr sockedId)
{
    qDebug() << "New connection";
    //add a new user
    users.push_back( new User(sockedId, *this) );
}

void TcpServer::getUserId(QString login)
{
    Q_UNUSED(login);
}

void TcpServer::registerSocketRequests(SocketHandler<TcpServer> &socketHandler)
{
    //here goes all callbacks to each request from client
    //functions must be listed in the same order as requests are listed in requests.h file
    std::vector< void (TcpServer::*)(QDataStream&, SocketHandler<TcpServer>*) > callbacks = {
        &TcpServer::onTextRequested
    };

    //register those callbacks in given socket handler
    for (unsigned int i = 0 ; i < callbacks.size() ; i++)
        socketHandler.addReadyReadCallback(static_cast<Requests>(i), callbacks[i]);
}

User &TcpServer::getUserBySocketHandler(SocketHandler<TcpServer> *socketHandler)
{
    unsigned int i;
    for (i = 0 ; &users[i]->socketHandler != socketHandler ; i++);
    return *users[i];
}

void TcpServer::onTextRequested(QDataStream &data, SocketHandler<TcpServer> *socketHandler)
{
    QString str;
    data >> str;
    qDebug() << "Text request: " << str << "\nId: " << getUserBySocketHandler(socketHandler).userId;
}
