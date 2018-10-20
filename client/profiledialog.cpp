#include "profiledialog.h"
#include <QMessageBox>
#include <QCloseEvent>

extern SocketHandler socketHandler;

ProfileDialog::ProfileDialog(UserData &userData, QWidget *parent) :
    QDialog(parent), editedUser(userData)
{
    ui.setupUi(this);
    registerRequestsReceiver(&socketHandler);
    displayUserData();
    connectInputSignals();
}

void ProfileDialog::displayUserData()
{
    ui.idEdit->setText( QString::number(editedUser.id) );
    ui.loginEdit->setText( editedUser.login );
    ui.nameEdit->setText( editedUser.name );
    ui.surnameEdit->setText( editedUser.surname );
    ui.companyNameEdit->setText( editedUser.companyName );
    ui.phoneEdit->setText( editedUser.phone );
    ui.emailEdit->setText( editedUser.email );
}

void ProfileDialog::connectInputSignals()
{
    QObject::connect(ui.nameEdit, SIGNAL(textChanged(QString)), this, SLOT(onInput()));
    QObject::connect(ui.surnameEdit, SIGNAL(textChanged(QString)), this, SLOT(onInput()));
    QObject::connect(ui.companyNameEdit, SIGNAL(textChanged(QString)), this, SLOT(onInput()));
    QObject::connect(ui.phoneEdit, SIGNAL(textChanged(QString)), this, SLOT(onInput()));
    QObject::connect(ui.emailEdit, SIGNAL(textChanged(QString)), this, SLOT(onInput()));
}

UserData ProfileDialog::getLineEditsData()
{
    UserData data;
    data.id = editedUser.id;
    data.login = editedUser.login;
    data.name = ui.nameEdit->text();
    data.surname = ui.surnameEdit->text();
    data.companyName = ui.companyNameEdit->text();
    data.phone = ui.phoneEdit->text();
    data.email = ui.emailEdit->text();
    return data;
}

void ProfileDialog::onCloseDialog()
{
    //if apply button is enabled there are unsaved changes
    if ( ui.applyBtn->isEnabled() )
    {
        int buttonNr;
        buttonNr = QMessageBox::question(this, "Niezapisane zmiany",
            "Czy napewno chcesz zamknąć to okno nie zapisując wprowadzonych zmian?", "Tak", "Nie", QString(), 1);
        //if yes was clicked close profile dialog
        if (!buttonNr)
            this->deleteLater();
    }
    else
    {
        //all saved, close immediately
        this->deleteLater();
    }
}

void ProfileDialog::sendChangeUserDataRequest()
{
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream << getLineEditsData();
    Request req(Request::CHANGE_USER_DATA, bytes);
    socketHandler.send(req);
}

void ProfileDialog::onChangeUserDataResponse(Request &req)
{
    //if errors occured while saving
    if (req.status == Request::ERROR)
    {
        QMessageBox::warning(this, "Błąd podczas zapisywania", "Nie udało się zapisać wprowadzonych zmian!");
        ui.applyBtn->setEnabled(true);
        return;
    }

    QDataStream stream(&req.data, QIODevice::ReadOnly);
    stream >> editedUser;
    QMessageBox::information(this, "Zapisano zmiany", "Zmiany zostały zapisane");
}

void ProfileDialog::on_okBtn_clicked()
{
    onCloseDialog();
}

void ProfileDialog::on_applyBtn_clicked()
{
    ui.applyBtn->setDisabled(true);
    sendChangeUserDataRequest();
}

void ProfileDialog::onInput()
{
    ui.applyBtn->setEnabled(true);
}

void ProfileDialog::closeEvent(QCloseEvent *e)
{
    e->ignore();
    onCloseDialog();
}

void ProfileDialog::onDataReceived(Request req, SocketHandler *)
{
    onChangeUserDataResponse(req);
}

void ProfileDialog::registerRequestsReceiver(SocketHandler *socketHandler)
{
    RequestReceiver::registerRequestsReceiver(socketHandler);
    socketHandler->addRequestReceiver(std::vector<Request::RequestName>({ Request::CHANGE_USER_DATA }), *this);
}
