#include "userswidget.h"
#include "../shared/sockethandler.h"
#include <QMessageBox>
#include "profiledialog.h"
#include "../shared/reader.h"

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

void UsersWidget::onDataReceived(Request req, SocketHandler*)
{
    if (req.name == Request::GET_ALL_USERS_DATA)
        onGetAllUsersDataResponse(req);
    else if (req.name == Request::DELETE_USER)
        onDeleteUserResponse(req);
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

UserData UsersWidget::rowToUserData(unsigned row)
{
    UserData data;
    data.id = ui.columnList->getFieldValue(row, 0).toUInt();
    data.login = ui.columnList->getFieldValue(row, 1);
    data.name = ui.columnList->getFieldValue(row, 2);
    data.surname = ui.columnList->getFieldValue(row, 3);
    data.companyName = ui.columnList->getFieldValue(row, 4);
    data.phone = ui.columnList->getFieldValue(row, 5);
    data.email = ui.columnList->getFieldValue(row, 6);

    return data;
}

void UsersWidget::requestGetAllUsersData()
{
    socketHandler.send( Request(getReceiverId(), Request::GET_ALL_USERS_DATA) );
}

void UsersWidget::requestDeleteUser()
{
    //send to server selected user's id
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream << static_cast<quint32>(ui.columnList->getFieldValue( static_cast<unsigned>(ui.columnList->getCurrentRow()), 0 ).toUInt());

    socketHandler.send( Request(getReceiverId(), Request::DELETE_USER, bytes) );
}

void UsersWidget::on_addUserBtn_clicked()
{

}

void UsersWidget::onEditUserClicked()
{
    UserData data = rowToUserData( static_cast<unsigned>(ui.columnList->getCurrentRow()) );
    ProfileDialog* profile = new ProfileDialog(data, this);
    QObject::connect(profile, SIGNAL(userDataChanged(UserData)), this, SLOT(onUserDataChanged(UserData)));
    profile->show();
}

void UsersWidget::onRemoveUserClicked()
{
    requestDeleteUser();
}

void UsersWidget::onUserDataChanged(UserData data)
{
    //set values to edited row
    unsigned row = static_cast<unsigned>(ui.columnList->getCurrentRow());
    ui.columnList->setFieldValue(row, 0, QString::number(data.id));
    ui.columnList->setFieldValue(row, 1, data.login);
    ui.columnList->setFieldValue(row, 2, data.name);
    ui.columnList->setFieldValue(row, 3, data.surname);
    ui.columnList->setFieldValue(row, 4, data.companyName);
    ui.columnList->setFieldValue(row, 5, data.phone);
    ui.columnList->setFieldValue(row, 6, data.email);
}

void UsersWidget::onGetAllUsersDataResponse(Request &req)
{
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się pobrać danych użytkowników!");
        return;
    }

    //retrieve users' data
    QVector<UserData> usersData = Reader(req).read<QVector<UserData>>();

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
