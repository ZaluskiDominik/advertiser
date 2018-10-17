#ifndef REQUESTS_H
#define REQUESTS_H

#include <QDataStream>
#include <QByteArray>

//class representing send request or response
class Request
{
    friend QDataStream& operator>>(QDataStream& stream, Request& req);
public:
    //list of available requests
    enum RequestName
    {
        //login authentication
        LOGIN_AUTH,
        GET_USER_DATA,

        //element used to retrieve number of requests, it's not a request!
        Count
    };

    //what request is this
    RequestName name;
    //content of request/response
    QByteArray data;

    Request() = default;
    Request(const RequestName& _name, const QByteArray& _data)
        :name(_name), data(_data)
    {
    }

    //cast operator to QbyteArray type
    operator QByteArray() const;
};

#endif // REQUESTS_H
