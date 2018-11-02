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
        //login authentication
        LOGIN_AUTH,
        CHANGE_USER_DATA,
        GET_ALL_USERS_DATA,
        DELETE_USER,
        GET_ACTIVE_PRICE_LIST,
        GET_ALL_PRICE_LISTS,
        CHANGE_ACTIVE_PRICE_LIST,

        //element used to retrieve number of requests, it's not a request!
        Count
    };

    //what request is this
    RequestName name;
    RequestStatus status;
    //content of request/response
    QByteArray data;

    //overloaded constructors
    Request() = default;
    Request(const RequestName& _name, const QByteArray& _data, const RequestStatus& _status)
        :name(_name), status(_status), data(_data)
    {
    }
    Request(const RequestName& _name, const RequestStatus& _status)
        :name(_name), status(_status)
    {
    }
    Request(const RequestName& _name, const QByteArray& _data = QByteArray())
        :name(_name), data(_data)
    {
    }

    //cast operator to QbyteArray type
    operator QByteArray() const;
};

#endif // REQUESTS_H
