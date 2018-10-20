#include "loginwidget.h"
#include "sockethandler.h"
#include "userdata.h"
#include <QMessageBox>

extern SocketHandler socketHandler;
extern UserData user;

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
    registerRequestsReceiver(&socketHandler);
}

void LoginWidget::onDataReceived(Request req, SocketHandler *)
{
    onLoginAuthResponse(req);
}

void LoginWidget::registerRequestsReceiver(SocketHandler *socketHandler)
{
    RequestReceiver::registerRequestsReceiver(socketHandler);

    std::vector<Request::RequestName> names({ Request::LOGIN_AUTH });
    socketHandler->addRequestReceiver(names, *this);
}

void LoginWidget::on_pushButton_clicked()
{
    //connect to server
    if ( !socketHandler.isConnected() )
        connectToServer();

    //send login authentication request
    sendLoginAuthRequest();
}

void LoginWidget::connectToServer()
{
    //if could't connect
    if ( !socketHandler.connectToHost("127.0.0.1", 1111, 500) )
    {
        //display error and exit app
        QMessageBox::warning(this, "Błąd", "Nie udało połączyć się z serwerem. Aplikacja zostanie zamknięta...");
        QCoreApplication::quit();
    }
}

void LoginWidget::sendLoginAuthRequest()
{
    //prepare request to authenticate login creadentials
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    //write login and password
    stream << ui.login->text() << ui.password->text();
    //send request
    socketHandler.send( Request(Request::LOGIN_AUTH, buffer) );
}

void LoginWidget::onLoginAuthResponse(Request& req)
{
    //if an error occured
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Wystąpił nieznany błąd podczas uwierzytelniania!");
        return;
    }

    QDataStream stream(&req.data, QIODevice::ReadOnly);
    bool isAuth;
    stream >> isAuth;

    //if login process was succesfull
    if (isAuth)
    {
        stream >> user;
        //clear login inputs
        ui.login->clear();
        ui.password->clear();
        //clear login error message
        ui.loginErr->setText("");
        //emit signal to main window class
        emit userLoggedIn();
    }
    else
    {
        //login or password wasn't found, show error message
        ui.loginErr->setText("Nieprawidłowy login lub hasło!");
    }
}
