#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

class DBManager
{
public:
    explicit DBManager();
    ~DBManager();

    //exeutes query and returns object from which data can be retrieved
    QSqlQuery* query(const QString& query);
    //prepared statements
    QSqlQuery* prepare(const QString& query);

    //returns number of returned records by sql query
    int rowCount(QSqlQuery *q);

    //whether a connection to database was opened successfully
    bool isOpen();

private:
    bool isDbOpen = false;

    //object responsible for executing sql queries
    QSqlQuery* sqlQuery;
};

#endif // DBMANAGER_H
