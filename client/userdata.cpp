#include "userdata.h"
#include <QByteArray>

UserData::operator QByteArray() const
{
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream << id << login << name << surname << companyName << email << phone << isAdmin;
    return bytes;
}

QDataStream& operator>>(QDataStream &stream, UserData &user)
{
    stream >> user.id >> user.login >> user.name >> user.surname >> user.companyName >> user.email >> user.phone >> user.isAdmin;
    return stream;
}
