#ifndef USERDATA_H
#define USERDATA_H

#include <QDataStream>
#include <QSqlQuery>

class UserData
{
public:
    UserData() = default;
    UserData(QSqlQuery* query);
    int id;
    QString login;
    QString name;
    QString surname;
    QString companyName;
    QString phone;
    QString email;
    bool isAdmin;

    operator QByteArray() const;
};

QDataStream& operator>>(QDataStream &stream, UserData &user);

#endif // USERDATA_H
