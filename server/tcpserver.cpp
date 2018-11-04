#include "tcpserver.h"
#include <QtDebug>
#include <QVector>
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
    //list all callbacks in the same order as requests in requests.h are listed
    responsesToRequests = std::vector< void (TcpServer::*)(Request&, SocketHandler*) >{
        &TcpServer::onLoginAuthRequest,
        &TcpServer::onChangeUserDataRequest,
        &TcpServer::onGetAllUsersData,
        &TcpServer::onDeleteUserRequest,
        &TcpServer::onGetActivePriceList,
        &TcpServer::onGetAllPriceListsRequest,
        &TcpServer::onChangeActivePriceListRequest,
        &TcpServer::onRemovePriceListRequest,
        &TcpServer::onSavePriceListRequest,
        &TcpServer::onAddNewPriceListRequest
    };
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

    //register that requests in socketHandler
    socketHandler->addRequestReceiver(*this);
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

    DBManager db;
    //if couldn't connect to db send error response to client
    if ( !db.isOpen() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    //check in database if user with that credentials exists
    QSqlQuery* query = db.prepare(QString("SELECT * FROM users LEFT JOIN ") +
    "admins ON admins.user_id = users.id WHERE users.login = ? AND users.password = ?");
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
    sendLoginAuthResponse(req.receiverId, socketHandler, userData, isAuth);
}

void TcpServer::sendLoginAuthResponse(quint32 receiverId, SocketHandler* socketHandler, const UserData &userData, bool isAuth)
{
    QByteArray resp;
    QDataStream out(&resp, QIODevice::WriteOnly);

    //whether login credentials are valid
    out << isAuth;
    //container for user data
    out << userData;

    socketHandler->send( Request(receiverId, Request::LOGIN_AUTH, resp, Request::OK) );
}

void TcpServer::onChangeUserDataRequest(Request& req, SocketHandler* socketHandler)
{
    User& user = getUserBySocketHandler(socketHandler);

    //check if request's sender has right to modify user's data(sender changes his own data or it's an admin)
    if (user.socketHandler == socketHandler || user.isAdmin)
    {
        //retrieve data that will be updated in db
        QDataStream stream(&req.data, QIODevice::ReadOnly);
        UserData userData;
        stream >> userData;

        //if data was saved
        if ( saveUserData(userData) )
        {
            //send to client updated user's data
            QByteArray outBuffer;
            QDataStream out(&outBuffer, QIODevice::WriteOnly);
            out << userData;
            socketHandler->send( Request(req.receiverId, req.name, outBuffer, Request::OK) );
        }
        else
        {
            //data could't be saved, send error response
            socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        }
    }
}

bool TcpServer::saveUserData(const UserData &userData)
{
    DBManager db;

    //if db isn't open send error response
    if ( !db.isOpen() )
        return false;

    //update user's data
    QSqlQuery* query = db.prepare(QString("UPDATE users SET name = ?, surname = ?, company_name = ?") +
        ", phone = ?, email = ? WHERE id = ?");
    query->addBindValue(userData.name);
    query->addBindValue(userData.surname);
    query->addBindValue(userData.companyName);
    query->addBindValue(userData.phone);
    query->addBindValue(userData.email);
    query->addBindValue(userData.id);
    query->exec();

    return query->isActive();
}

void TcpServer::onGetAllUsersData(Request &req, SocketHandler *socketHandler)
{
    DBManager db;

    //if db isn't open send error response
    if ( !db.isOpen() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    //get all records of users excluding admins
    QSqlQuery* query = db.query(QString("SELECT users.* FROM users LEFT JOIN admins ") +
        "ON admins.user_id = users.id WHERE admins.user_id IS NULL");

    //if query failed
    if ( !query->isActive() )
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
    else
    {
        //query was successfull, create array with users' data
        QVector<UserData> users;
        while ( query->next() )
            users.push_back(UserData(query));

        //send response containing users' data
        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out << users;
        socketHandler->send( Request(req.receiverId, req.name, bytes, Request::OK) );
    }
}

void TcpServer::onDeleteUserRequest(Request &req, SocketHandler *socketHandler)
{
    DBManager db;

    //if db isn't open send error response
    if ( !db.isOpen() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    //retrieve id of user that will be deleted
    QDataStream in(&req.data, QIODevice::ReadOnly);
    quint32 userId;
    in >> userId;

    //delete user with that id from db
    QSqlQuery* query = db.prepare("DELETE FROM users WHERE id = ?");
    query->addBindValue(userId);
    query->exec();

    Request::RequestStatus status = ( query->isActive() ) ? Request::OK : Request::ERROR;
    socketHandler->send( Request(req.receiverId, req.name, status) );
}

void TcpServer::onGetActivePriceList(Request &req, SocketHandler *socketHandler)
{
    DBManager db;

    //if db isn't open send error response
    if ( !db.isOpen() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    //get all data about active price list
    QSqlQuery* query = db.query(QString("SELECT p.*, a.list_id FROM price_lists AS p INNER JOIN active_price_list AS a ") +
        "ON a.list_id = p.price_list_id ORDER BY p.hours");

    //if query failed
    if (!query->isActive())
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
    else
    {
        //query successed
        //send response containing price list
        QByteArray resp;
        QDataStream out(&resp, QIODevice::WriteOnly);
        out << convertToPriceList(query);
        socketHandler->send( Request(req.receiverId, req.name, resp, Request::OK) );
    }
}

PriceList TcpServer::convertToPriceList(QSqlQuery *query)
{
    PriceList prices;

    //add rows
    while (query->next())
    {
        PriceListRow row;
        row.hours = query->value(query->record().indexOf("hours")).toString();
        row.weekPrice = query->value(query->record().indexOf("week_price")).toUInt();
        row.weekendPrice = query->value(query->record().indexOf("weekend_price")).toUInt();
        prices.rows.push_back(row);
    }

    //set id of a price list
    query->first();
    prices.priceListId = query->value(query->record().indexOf("price_list_id")).toUInt();
    //info if it's an active price list
    prices.isActive = !query->isNull( query->record().indexOf("list_id") );

    return prices;
}

void TcpServer::onGetAllPriceListsRequest(Request &req, SocketHandler *socketHandler)
{
    DBManager db;

    //if db isn't open send error response
    if ( !db.isOpen() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    //get all price lists' ids
    QSqlQuery* query = db.query("SELECT price_list_id FROM price_lists GROUP BY price_list_id");

    //if query failed to execute
    if ( !query->isActive() )
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
    else
    {
        //convert all ids from query to vector of ids
        QVector<int> ids;
        while (query->next())
            ids.push_back( query->value(0).toInt() );

        QVector<PriceList> priceLists;
        //for each id, get this price list content
        for (auto i = ids.begin() ; i != ids.end() ; i++)
        {
            //get content of currently iterated price list
            query = db.prepare(QString("SELECT p.*, a.list_id FROM price_lists AS p LEFT JOIN ") +
                "active_price_list AS a ON a.list_id = p.price_list_id WHERE p.price_list_id = ? ORDER BY p.hours");
            query->addBindValue(*i);
            query->exec();

            //if query failed
            if ( !query->isActive() )
            {
                socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
                return;
            }

            //add price list returned by query to price lists array
            priceLists.push_back( convertToPriceList(query) );
        }

        //send response
        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out << priceLists;
        socketHandler->send( Request(req.receiverId, req.name, bytes, Request::OK) );
    }
}

void TcpServer::onChangeActivePriceListRequest(Request &req, SocketHandler *socketHandler)
{
    DBManager db;

    //if db isn't open send error response
    if ( !db.isOpen() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    QDataStream in(&req.data, QIODevice::ReadOnly);
    quint32 priceListId;
    in >> priceListId;

    //run query setting price list with priceListId as active
    QSqlQuery* query = db.prepare("UPDATE active_price_list SET list_id = ?");
    query->addBindValue(priceListId);
    query->exec();

    //if query failed to execute, send error status, else send ok status
    Request::RequestStatus status = ( query->isActive() ) ? Request::OK : Request::ERROR;
    socketHandler->send( Request(req.receiverId, req.name, status) );
}

void TcpServer::onRemovePriceListRequest(Request &req, SocketHandler *socketHandler)
{
    DBManager db;

    //if db isn't open send error response
    if ( !db.isOpen() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    QDataStream in(&req.data, QIODevice::ReadOnly);
    quint32 priceListId;
    in >> priceListId;

    //run query deleting price list
    QSqlQuery* query = db.prepare("DELETE FROM price_lists WHERE price_list_id = ?");
    query->addBindValue(priceListId);
    query->exec();

    //if query failed to execute, send error status, else send ok status
    Request::RequestStatus status = ( query->isActive() ) ? Request::OK : Request::ERROR;
    socketHandler->send( Request(req.receiverId, req.name, status) );
}

void TcpServer::onSavePriceListRequest(Request &req, SocketHandler *socketHandler)
{
    DBManager db;

    //if db isn't open send error response
    if ( !db.isOpen() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    QDataStream in(&req.data, QIODevice::ReadOnly);
    PriceList prices;
    in >> prices;

    //prepare statement updating each row of price list that client want to modify
    QSqlQuery* query = db.prepare(QString("UPDATE price_lists SET week_price = ?, weekend_price = ?") +
        " WHERE price_list_id = ? AND hours = ?");

    //execute that statement for each hours period
    bool success = true;
    for (auto i = prices.rows.begin() ; i != prices.rows.end() ; i++)
    {
        query->addBindValue(i->weekPrice);
        query->addBindValue(i->weekendPrice);
        query->addBindValue(prices.priceListId);
        query->addBindValue(i->hours);
        query->exec();
        success &= query->isActive();
    }

    //if query failed to execute, send error status, else send ok status
    Request::RequestStatus status = ( success ) ? Request::OK : Request::ERROR;
    socketHandler->send( Request(req.receiverId, req.name, status) );
}

void TcpServer::onAddNewPriceListRequest(Request &req, SocketHandler *socketHandler)
{
    DBManager db;

    //if db isn't open send error response
    if ( !db.isOpen() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    QDataStream in(&req.data, QIODevice::ReadOnly);
    PriceList prices;
    in >> prices;

    //find max price list's id from db
    QSqlQuery* query = db.query("SELECT MAX(price_list_id) FROM price_lists");
    if ( !query->isActive() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    //set id of new price list to found max id + 1
    query->first();
    int newPriceListId = query->value(0).toInt() + 1;

    //insert new price list's values
    query->prepare("INSERT INTO price_lists VALUES(null, ?, ?, ?, ?)");
    for (auto i = prices.rows.begin() ; i != prices.rows.end() ; i++)
    {
        query->addBindValue(newPriceListId);
        query->addBindValue(i->hours);
        query->addBindValue(i->weekPrice);
        query->addBindValue(i->weekendPrice);
        query->exec();

        if ( !query->isActive() )
        {
            query->prepare("DELETE FROM price_lists WHERE price_list_id = ?");
            query->addBindValue(newPriceListId);
            query->exec();

            socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
            return;
        }
    }

    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    prices.priceListId = static_cast<quint32>(newPriceListId);
    out << prices;
    socketHandler->send( Request(req.receiverId, req.name, bytes, Request::OK) );
}
