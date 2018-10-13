#include "requests.h"

QDataStream& operator>>(QDataStream& stream, Request& req)
{
    quint32 reqCode;
    QByteArray bytes;
    stream >> reqCode;
    stream >> bytes;
    req.name = static_cast<Request::RequestName>(reqCode);
    req.data = bytes;
    return stream;
}

Request::operator QByteArray() const
{
    QByteArray bytes;
    QDataStream stream (&bytes, QIODevice::WriteOnly);
    stream << static_cast<quint32>(name);
    stream << data;
    return bytes;
}
