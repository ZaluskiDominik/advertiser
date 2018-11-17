#ifndef USERSWIDGET_H
#define USERSWIDGET_H

#include <QWidget>
#include "ui_userswidget.h"
#include "columnlistwidget.h"
#include <QHBoxLayout>
#include "../shared/requestreceiver.h"
#include "../shared/userdata.h"

class UsersWidget : public QWidget, public RequestReceiver
{
    Q_OBJECT

public:
    explicit UsersWidget(QWidget *parent = nullptr);

protected:
    void onDataReceived(Request req, SocketHandler*) final;

private slots:
    //add new user button clicked
    void on_addUserBtn_clicked();

    //edit user's data in context menu was clicked
    void onEditUserClicked();
    //remove user in context menu was clicked
    void onRemoveUserClicked();

    //user's data was edited in profileDialog
    void onUserDataChanged(UserData data);

    //response from server with all users' data
    void onGetAllUsersDataResponse(Request& req);

    //response from server with info whether user was successfully deleted from db
    void onDeleteUserResponse(Request& req);

private:
    Ui::UsersWidget ui;

    //creates columns named after user's data fields
    void initColumns();

    //connect edit and remove signals to appropriate slots
    void connectSlots();

    //returns UserData object created from given row's data
    UserData rowToUserData(unsigned row);

    //sends request to server in order to receive data of all users(excluding admins)
    void requestGetAllUsersData();

    //sends request to server in order to delete currently highlighted user from db
    void requestDeleteUser();
};

#endif // USERSWIDGET_H
