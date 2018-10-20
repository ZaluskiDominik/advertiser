#ifndef USERDATA_H
#define USERDATA_H

#include <QDataStream>

class UserData
{
public:
    quint32 id;
    QString login;
    QString name;
    QString surname;
    QString companyName;
    QString phone;
    QString email;
    bool isAdmin;
};

QDataStream& operator>>(QDataStream &stream, UserData &user);
QDataStream& operator<<(QDataStream &stream, const UserData &user);

#endif // USERDATA_H
