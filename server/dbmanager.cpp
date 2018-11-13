#include "dbmanager.h"
#include <QStringList>

DBManager::DBManager()
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("advertiser.db");
    //if couldn't connect with db
    if ( !db.open() )
    {
        isDbOpen = false;
        return;
    }
    isDbOpen = true;

    //create QSqlQuery instance
    sqlQuery = new QSqlQuery;
}

DBManager::~DBManager()
{
    delete sqlQuery;
    QSqlDatabase::database(QSqlDatabase::defaultConnection).close();
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

QSqlQuery *DBManager::query(const QString &query)
{
    sqlQuery->exec(query);
    return sqlQuery;
}

QSqlQuery *DBManager::prepare(const QString &query)
{
    sqlQuery->prepare(query);
    return sqlQuery;
}

int DBManager::rowCount(QSqlQuery* q)
{
    int counter = 0;
    while ( q->next() )
        counter++;
    q->seek(-1);
    return counter;
}

bool DBManager::isOpen()
{
    return isDbOpen;
}
