#include "profiledialog.h"

ProfileDialog::ProfileDialog(UserData &userData, QWidget *parent) :
    QDialog(parent), editedUser(userData)
{
    ui.setupUi(this);
}

ProfileDialog::~ProfileDialog()
{
}
