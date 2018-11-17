#ifndef REQUESTS_H
#define REQUESTS_H

#include <QDataStream>
#include <QByteArray>

//class representing send request or response
class Request
{
    friend QDataStream& operator>>(QDataStream& stream, Request& req);
public:
    //status of request
    enum RequestStatus
    {
        OK,
        ERROR
    };

    //list of available requests
    enum RequestName
    {
        //login authentication(login, password)
        LOGIN_AUTH,
        GET_USER_DATA,
        //change data of given user(user's id)
        CHANGE_USER_DATA,
        //get list of all regular users
        GET_ALL_USERS_DATA,
        //remove given user from db
        DELETE_USER,
        //get price list that is currently used
        GET_ACTIVE_PRICE_LIST,
        //get all price list stored in db
        GET_ALL_PRICE_LISTS,
        //set choosen price list as active
        CHANGE_ACTIVE_PRICE_LIST,
        //remove given price list from db
        REMOVE_PRICE_LIST,
        //save given price list
        SAVE_PRICE_LIST,
        //ad a new price list
        ADD_NEW_PRICE_LIST,
        //get all ads stored in db
        GET_ADS,
        ADD_NEW_AD,
        MODIFY_AD,
        REMOVE_AD,

        //element used to retrieve number of requests, it's not a request!
        Count
    };

    //number that will identify RequestReceiver instance to which response will be send
    quint32 receiverId;
    //what request is this
    RequestName name;
    RequestStatus status;
    //content of request/response
    QByteArray data;

    //overloaded constructors
    Request() = default;
    Request(const quint32& _receiverId, const RequestName& _name, const QByteArray& _data, const RequestStatus& _status)
        :receiverId(_receiverId), name(_name), status(_status), data(_data)
    {
    }
    Request(const quint32& _receiverId, const RequestName& _name, const RequestStatus& _status)
        :receiverId(_receiverId), name(_name), status(_status)
    {
    }
    Request(const quint32& _receiverId, const RequestName& _name, const QByteArray& _data = QByteArray())
        :receiverId(_receiverId), name(_name), data(_data)
    {
    }

    //cast operator to QbyteArray type
    operator QByteArray() const;
};

#endif // REQUESTS_H
