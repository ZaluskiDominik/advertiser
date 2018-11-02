#include "requests.h"

QDataStream& operator>>(QDataStream& stream, Request& req)
{
    quint32 reqCode;
    stream >> reqCode;
    req.name = static_cast<Request::RequestName>(reqCode);
    stream >> req.data;
    stream >> req.receiverId;
    quint32 statusCode;
    stream >> statusCode;
    req.status = static_cast<Request::RequestStatus>(statusCode);

    return stream;
}

Request::operator QByteArray() const
{
    QByteArray bytes;
    QDataStream stream (&bytes, QIODevice::WriteOnly);
    stream << static_cast<quint32>(name);
    stream << data;
    stream << receiverId;
    stream << static_cast<quint32>(status);

    return bytes;
}
