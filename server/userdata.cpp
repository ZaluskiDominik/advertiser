#include "userdata.h"
#include <QByteArray>
#include <QSqlRecord>
#include <QVariant>

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

UserData::UserData(QSqlQuery *query)
{
    id = query->value( query->record().indexOf("id") ).toUInt();
    login = query->value( query->record().indexOf("login") ).toString();
    name = query->value( query->record().indexOf("name") ).toString();
    surname = query->value( query->record().indexOf("surname") ).toString();
    companyName = query->value( query->record().indexOf("company_name") ).toString();
    email = query->value( query->record().indexOf("email") ).toString();
    phone = query->value( query->record().indexOf("phone") ).toString();
    isAdmin = !( query->isNull( query->record().indexOf("user_id") ) );
}
