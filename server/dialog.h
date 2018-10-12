#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "tcpserver.h"

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    TcpServer server;
};

#endif // DIALOG_H
