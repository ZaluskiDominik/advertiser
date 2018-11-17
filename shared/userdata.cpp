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

UserData::UserData(const UserData &other)
{
    copyData(other);
}

UserData &UserData::operator=(const UserData &other)
{
    copyData(other);
    return *this;
}

void UserData::copyData(const UserData &other)
{
    if (this == &other)
        return;

    id = other.id;
    login = other.login;
    name = other.name;
    surname = other.surname;
    companyName = other.companyName;
    email = other.email;
    phone = other.phone;
}
