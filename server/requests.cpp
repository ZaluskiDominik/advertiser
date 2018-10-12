#include "requests.h"

QDataStream& operator>>(QDataStream& stream, Requests& req)
{
    quint32 reqCode;
    stream >> reqCode;
    req = static_cast<Requests>(req);
    return stream;
}

QDataStream& operator<<(QDataStream& stream, const Requests req)
{
    quint32 reqCode = static_cast<quint32>(req);
    stream << reqCode;
    return stream;
}
