#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include "ui_profiledialog.h"
#include "userdata.h"

class ProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProfileDialog(UserData& userData, QWidget *parent = nullptr);
    ~ProfileDialog();

private:
    Ui::ProfileDialog ui;

    UserData& editedUser;
};

#endif // PROFILEDIALOG_H
