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
    bool isAdmin = false;

    UserData() = default;
    UserData(const UserData& other);
    UserData& operator=(const UserData& other);

private:
    //copies data from other UserData object(excluding isAdmin var)
    //used in copy constructor and assignment operator
    void copyData(const UserData& other);
};

QDataStream& operator>>(QDataStream &stream, UserData &user);
QDataStream& operator<<(QDataStream &stream, const UserData &user);

#endif // USERDATA_H
