#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include "ui_profiledialog.h"
#include "userdata.h"
#include "sockethandler.h"

class ProfileDialog : public QDialog, public RequestReceiver
{
    Q_OBJECT

public:
    explicit ProfileDialog(UserData& userData, QWidget *parent = nullptr);

private slots:
    //ok button clicked
    void on_okBtn_clicked();
    //apply button clicked
    void on_applyBtn_clicked();

    //text content of line edit changed
    void onInput();

protected:
    //display message before closing dialog if there are unsaved changes
    void closeEvent(QCloseEvent* e);

    virtual void onDataReceived(Request req, SocketHandler*) final;
    virtual void registerRequestsReceiver(SocketHandler* socketHandler) final;

private:
    Ui::ProfileDialog ui;

    //user data that was supplied in constructor, and will be changed after apply button click
    UserData& editedUser;

    //set text to each line edit with user's data
    void displayUserData();

    //connect all line edits changed signals to onInput() slot
    void connectInputSignals();

    //return UserData object constructed from line edits' text
    UserData getLineEditsData();

    //ok button was clicked or x sign
    void onCloseDialog();

    //sends data typed by user in profile dialog to server, which will save it in db
    void sendChangeUserDataRequest();
    void onChangeUserDataResponse(Request& req);
};

#endif // PROFILEDIALOG_H
