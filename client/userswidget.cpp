#include "userswidget.h"
#include "sockethandler.h"
#include <QMessageBox>

extern SocketHandler socketHandler;

UsersWidget::UsersWidget(QWidget *parent)
    :QWidget(parent)
{
    ui.setupUi(this);
    registerRequestsReceiver(&socketHandler);
    initColumns();
    connectSlots();
    requestGetAllUsersData();
}

UsersWidget::~UsersWidget()
{
}

void UsersWidget::onDataReceived(Request req, SocketHandler*)
{
    if (req.name == Request::GET_ALL_USERS_DATA)
        onGetAllUsersDataResponse(req);
    else if (req.name == Request::DELETE_USER)
        onDeleteUserResponse(req);
}

void UsersWidget::registerRequestsReceiver(SocketHandler *socketHandler)
{
    RequestReceiver::registerRequestsReceiver(socketHandler);

    socketHandler->addRequestReceiver(
        std::vector<Request::RequestName>{ Request::GET_ALL_USERS_DATA, Request::DELETE_USER }, *this);
}

void UsersWidget::initColumns()
{
    ui.columnList->addColumn("Id");
    ui.columnList->addColumn("Login");
    ui.columnList->addColumn("Imię");
    ui.columnList->addColumn("Nazwisko");
    ui.columnList->addColumn("Nazwa firmy");
    ui.columnList->addColumn("Telefon");
    ui.columnList->addColumn("Email");
}

void UsersWidget::connectSlots()
{
    QObject::connect(ui.columnList, SIGNAL(editClicked()), this, SLOT(onEditUserClicked()));
    QObject::connect(ui.columnList, SIGNAL(removeClicked()), this, SLOT(onRemoveUserClicked()));
}

void UsersWidget::requestGetAllUsersData()
{
    socketHandler.send( Request(Request::GET_ALL_USERS_DATA) );
}

void UsersWidget::requestDeleteUser()
{
    //send to server selected user's id
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream << ui.columnList->getFieldValue( static_cast<unsigned>(ui.columnList->getCurrentRow()), 0 );
    socketHandler.send( Request(Request::DELETE_USER, bytes) );
}

void UsersWidget::on_addUserBtn_clicked()
{

}

void UsersWidget::onEditUserClicked()
{

}

void UsersWidget::onRemoveUserClicked()
{
    requestDeleteUser();
}

void UsersWidget::onGetAllUsersDataResponse(Request &req)
{
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się pobrać danych użytkowników!");
        return;
    }

    //retrieve users' data
    QDataStream stream(&req.data, QIODevice::ReadOnly);
    QVector<UserData> usersData;
    stream >> usersData;

    //add rows with those data to ColumnListWidget instance
    for (auto i = usersData.begin() ; i != usersData.end() ; i++)
    {
        ui.columnList->addRow( std::vector<QString> {
            QString::number(i->id), i->login, i->name, i->surname, i->companyName,
            i->phone, i->email
        });
    }
}

void UsersWidget::onDeleteUserResponse(Request &req)
{
    if (req.status == Request::ERROR)
    {
        //error, user was't removed
        QMessageBox::warning(this, "Błąd", "Nie udało się usunąć wybranego użytkownika!");
        return;
    }
    else
    {
        //user succesfully remove from db
        ui.columnList->removeCurrentRow();
    }
}
