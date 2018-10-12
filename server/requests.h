#ifndef REQUESTS_H
#define REQUESTS_H

#include <QDataStream>

enum class Requests
{
    GET_TEXT,
    GET_COLOR
};

QDataStream& operator<<(QDataStream& stream, const Requests req);
QDataStream& operator>>(QDataStream& stream, Requests& req);

#endif // REQUESTS_H
