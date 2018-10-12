#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H
#include <QTcpSocket>
#include <vector>

class ConnectionHandler
{
public:
    ConnectionHandler();

protected:
    QTcpSocket* socket;

private:


    void onReadyRead();
};

#endif // CONNECTIONHANDLER_H
