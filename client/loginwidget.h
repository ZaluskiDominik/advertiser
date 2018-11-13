#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include "ui_loginwidget.h"
#include "../shared/requestreceiver.h"
#include "userdata.h"

class LoginWidget : public QWidget, public RequestReceiver
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);

protected:
    void onDataReceived(Request req, SocketHandler*) final;
    void registerRequestsReceiver(SocketHandler* socketHandler) final;

private slots:
    //login button clicked
    void on_pushButton_clicked();

private:
    Ui::LoginWidget ui;

    //starts connection with server
    void connectToServer();

    //send login's credentials to server in order to check if there are valid
    void sendLoginAuthRequest();

    //handle response from server to login request
    void onLoginAuthResponse(Request &req);

signals:
    void userLoggedIn();
};

#endif // LOGINWIDGET_H
