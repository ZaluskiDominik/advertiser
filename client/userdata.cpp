#include "userdata.h"
#include <QByteArray>

QDataStream& operator>>(QDataStream &stream, UserData &user)
{
    stream >> user.id >> user.login >> user.name >> user.surname >> user.companyName >> user.email >> user.phone >> user.isAdmin;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const UserData &user)
{
    stream << user.id << user.login << user.name << user.surname << user.companyName << user.email << user.phone << user.isAdmin;
    return stream;
}
