#include "tcpserver.h"
#include <QtDebug>
#include <QVector>
#include "../shared/adinfo.h"
#include "../shared/time.h"
#include "../shared/reader.h"

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
    DBManager db;
    //if couldn't connect to db send error response to client
    if ( !db.isOpen() )
    {
        sender->send( Request(request.receiverId, request.name, Request::ERROR) );
        return;
    }

    (this->*responsesToRequests[request.name])(request, sender, db);
}

void TcpServer::initResponsesToRequests()
{
    //list all callbacks in the same order as requests in requests.h are listed
    responsesToRequests = std::vector< void (TcpServer::*)(Request&, SocketHandler*, DBManager&) >{
        &TcpServer::onLoginAuthRequest,
        &TcpServer::onGetUserDataRequest,
        &TcpServer::onChangeUserDataRequest,
        &TcpServer::onGetAllUsersData,
        &TcpServer::onDeleteUserRequest,
        &TcpServer::onGetActivePriceList,
        &TcpServer::onGetAllPriceListsRequest,
        &TcpServer::onChangeActivePriceListRequest,
        &TcpServer::onRemovePriceListRequest,
        &TcpServer::onSavePriceListRequest,
        &TcpServer::onAddNewPriceListRequest,
        &TcpServer::onGetAdsRequest,
        &TcpServer::onAddNewAdRequest,
        &TcpServer::onModifyAdRequest,
        &TcpServer::onRemoveAdRequest
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

User &TcpServer::getUserBySocketHandler(SocketHandler *socketHandler)
{
    unsigned int i;
    for (i = 0 ; users[i]->socketHandler != socketHandler ; i++);
    return *users[i];
}

//RESPONSE TO REQUESTS

void TcpServer::onLoginAuthRequest(Request &req, SocketHandler *socketHandler, DBManager& db)
{
    User& user = getUserBySocketHandler(socketHandler);

    //extract login credentials
    QString login, password;
    QDataStream in(&req.data, QIODevice::ReadOnly);
    in >> login >> password;

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
        userData = convertToUserData(query);
        //set flag whether user is an admin
        user.isAdmin = userData.isAdmin = !( query->isNull( query->record().indexOf("user_id") ) );;
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

void TcpServer::onGetUserDataRequest(Request &req, SocketHandler *socketHandler, DBManager &db)
{
    quint32 userId = Reader(req).read<quint32>();

    QSqlQuery* query = db.prepare("SELECT * FROM users WHERE id = ?");
    query->addBindValue(userId);
    query->exec();

    if (query->isActive())
    {
        query->first();
        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out << convertToUserData(query);

        socketHandler->send( Request(req.receiverId, req.name, bytes, Request::OK) );
    }
    else
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
}

UserData TcpServer::convertToUserData(QSqlQuery *query)
{
    UserData data;

    data.id = query->value( query->record().indexOf("id") ).toUInt();
    data.login = query->value( query->record().indexOf("login") ).toString();
    data.name = query->value( query->record().indexOf("name") ).toString();
    data.surname = query->value( query->record().indexOf("surname") ).toString();
    data.companyName = query->value( query->record().indexOf("company_name") ).toString();
    data.email = query->value( query->record().indexOf("email") ).toString();
    data.phone = query->value( query->record().indexOf("phone") ).toString();

    return data;
}

void TcpServer::onChangeUserDataRequest(Request& req, SocketHandler* socketHandler, DBManager& db)
{
    //retrieve data that will be updated in db
    UserData userData = Reader(req).read<UserData>();

    //if data was saved
    if ( saveUserData(userData, db) )
    {
        //send to client updated user's data
        socketHandler->send( Request(req.receiverId, req.name, req.data, Request::OK) );
    }
    else
    {
        //data could't be saved, send error response
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
    }
}

bool TcpServer::saveUserData(const UserData &userData, DBManager& db)
{
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

void TcpServer::onGetAllUsersData(Request &req, SocketHandler *socketHandler, DBManager& db)
{
    //get all records of users excluding admins
    QSqlQuery* query = db.query(QString("SELECT users.* FROM users LEFT JOIN admins ") +
        "ON admins.user_id = users.id WHERE admins.user_id IS NULL");

    //if query failed
    if ( !query->isActive() )
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
    else
    {
        //query was successfull, create array holding users' data
        QVector<UserData> users;
        while ( query->next() )
            users.push_back( convertToUserData(query) );

        //send response containing users' data
        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out << users;
        socketHandler->send( Request(req.receiverId, req.name, bytes, Request::OK) );
    }
}

void TcpServer::onDeleteUserRequest(Request &req, SocketHandler *socketHandler, DBManager& db)
{
    //retrieve id of user that will be deleted
    quint32 userId = Reader(req).read<quint32>();

    //delete user with that id from db
    QSqlQuery* query = db.prepare("DELETE FROM users WHERE id = ?");
    query->addBindValue(userId);
    query->exec();

    //if user was succesfully deleted, remove all ads of that user
    if (query->isActive())
    {
        query->prepare("DELETE FROM ads WHERE user_id = ?");
        query->addBindValue(userId);
        query->exec();
    }

    Request::RequestStatus status = ( query->isActive() ) ? Request::OK : Request::ERROR;
    socketHandler->send( Request(req.receiverId, req.name, status) );
}

void TcpServer::onGetActivePriceList(Request &req, SocketHandler *socketHandler, DBManager& db)
{
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

void TcpServer::onGetAllPriceListsRequest(Request &req, SocketHandler *socketHandler, DBManager& db)
{
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

void TcpServer::onChangeActivePriceListRequest(Request &req, SocketHandler *socketHandler, DBManager& db)
{
    quint32 priceListId = Reader(req).read<quint32>();

    //run query setting price list with priceListId as active
    QSqlQuery* query = db.prepare("UPDATE active_price_list SET list_id = ?");
    query->addBindValue(priceListId);
    query->exec();

    //if query failed to execute, send error status, else send ok status
    Request::RequestStatus status = ( query->isActive() ) ? Request::OK : Request::ERROR;
    socketHandler->send( Request(req.receiverId, req.name, status) );
}

void TcpServer::onRemovePriceListRequest(Request &req, SocketHandler *socketHandler, DBManager& db)
{
    quint32 priceListId = Reader(req).read<quint32>();

    //run query deleting price list
    QSqlQuery* query = db.prepare("DELETE FROM price_lists WHERE price_list_id = ?");
    query->addBindValue(priceListId);
    query->exec();

    //if query failed to execute, send error status, else send ok status
    Request::RequestStatus status = ( query->isActive() ) ? Request::OK : Request::ERROR;
    socketHandler->send( Request(req.receiverId, req.name, status) );

    //remove all ads associated with that price list
    query->prepare("DELETE FROM ads WHERE price_list_id = ?");
    query->addBindValue(priceListId);
    query->exec();
}

void TcpServer::onSavePriceListRequest(Request &req, SocketHandler *socketHandler, DBManager& db)
{
    PriceList prices = Reader(req).read<PriceList>();

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

void TcpServer::onAddNewPriceListRequest(Request &req, SocketHandler *socketHandler, DBManager& db)
{
    PriceList prices = Reader(req).read<PriceList>();

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

void TcpServer::onGetAdsRequest(Request &req, SocketHandler *socketHandler, DBManager& db)
{
    //get all ads created for active price list
    QSqlQuery* query = db.query(QString("SELECT ads.* FROM ads INNER JOIN active_price_list AS list") +
        " ON list.list_id = ads.price_list_id");

    //if query failed
    if ( !query->isActive() )
    {
        socketHandler->send( Request(req.receiverId, req.name, Request::ERROR) );
        return;
    }

    QVector<AdInfo> ads;
    while (query->next())
    {
          AdInfo ad;
          ad.adId = query->value(query->record().indexOf("id")).toUInt();
          ad.ownerId = query->value(query->record().indexOf("user_id")).toUInt();
          ad.startHour = Time( query->value(query->record().indexOf("start_hour")).toString() );
          ad.endHour = Time( query->value( query->record().indexOf("end_hour")).toString() );
          ad.weekDayNr = query->value(query->record().indexOf("week_day_nr")).toUInt();
          ads.push_back(ad);
    }

    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << ads;

    socketHandler->send( Request(req.receiverId, req.name, bytes, Request::OK) );
}

void TcpServer::adDataChangeHelper(Request &req, SocketHandler *socketHandler, QSqlQuery* query, bool newAd)
{
    Reader reader(req);
    quint32 priceListId = reader.read<quint32>();
    AdInfo adInfo = reader.read<AdInfo>();

    //add values to prepared query passed by function parameter
    query->addBindValue(adInfo.ownerId);
    query->addBindValue(adInfo.weekDayNr);
    query->addBindValue(adInfo.startHour.getFullHour());
    query->addBindValue(adInfo.endHour.getFullHour());
    query->addBindValue(priceListId);

    if ( !newAd )
        query->addBindValue(adInfo.adId);

    query->exec();

    //prepare AdInfo object to send
    if (newAd)
        adInfo.adId = query->lastInsertId().toUInt();
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out << adInfo;

    //send response
    Request::RequestStatus status = ( query->isActive() ) ? Request::OK : Request::ERROR;
    socketHandler->send( Request(req.receiverId, req.name, bytes, status) );
}

void TcpServer::onAddNewAdRequest(Request &req, SocketHandler *socketHandler, DBManager& db)
{
    QSqlQuery* query = db.prepare("INSERT INTO ads VALUES(null, ?, ?, ?, ?, ?)");
    adDataChangeHelper(req, socketHandler, query, true);
}

void TcpServer::onModifyAdRequest(Request &req, SocketHandler *socketHandler, DBManager &db)
{
    QSqlQuery* query = db.prepare(QString("UPDATE ads SET user_id = ?, week_day_nr = ?, ") +
        "start_hour = ?, end_hour = ?, price_list_id = ? WHERE id = ?");
    adDataChangeHelper(req, socketHandler, query, false);
}

void TcpServer::onRemoveAdRequest(Request &req, SocketHandler *socketHandler, DBManager &db)
{
    //get id od ad that will be deleted
    quint32 adId = Reader(req).read<quint32>();

    //run query deleting ad
    QSqlQuery* query = db.prepare("DELETE FROM ads WHERE id = ?");
    query->addBindValue(adId);
    query->exec();

    //send response to client if removal was succesfull
    Request::RequestStatus status = ( query->isActive() ) ? Request::OK : Request::ERROR;
    socketHandler->send( Request(req.receiverId, req.name, req.data, status) );
}
