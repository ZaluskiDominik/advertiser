#include "admindialog.h"

AdminDialog::AdminDialog(QWidget *parent)
    :QDialog(parent)
{
    ui.setupUi(this);
    initStackedWidget();
}

AdminDialog::~AdminDialog()
{
}

void AdminDialog::initStackedWidget()
{
    //add widgets
    ui.stackedWidget->addWidget(&usersWidget);

    //set default widget to usersWidget
    ui.stackedWidget->setCurrentWidget(&usersWidget);
}

void AdminDialog::on_usersBtn_clicked()
{
    ui.stackedWidget->setCurrentWidget(&usersWidget);
}
