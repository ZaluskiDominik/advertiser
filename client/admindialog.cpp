#include "admindialog.h"

AdminDialog::AdminDialog(QWidget *parent)
    :QDialog(parent)
{
    ui.setupUi(this);
    initStackedWidget();

    setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint
                   | Qt::WindowCloseButtonHint);
}

void AdminDialog::initStackedWidget()
{
    //add widgets
    ui.stackedWidget->addWidget(&usersWidget);
    ui.stackedWidget->addWidget(&pricesWidget);
    ui.stackedWidget->addWidget(&adsTableWidget);

    //set default widget to usersWidget
    ui.stackedWidget->setCurrentWidget(&usersWidget);
}

void AdminDialog::on_usersBtn_clicked()
{
    ui.stackedWidget->setCurrentWidget(&usersWidget);
}

void AdminDialog::on_priceListsBtn_clicked()
{
    ui.stackedWidget->setCurrentWidget(&pricesWidget);
}

void AdminDialog::on_adsBtn_clicked()
{
    ui.stackedWidget->setCurrentWidget(&adsTableWidget);
}
