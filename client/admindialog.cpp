#include "admindialog.h"

AdminDialog::AdminDialog(QWidget *parent)
    :QDialog(parent)
{
    ui.setupUi(this);
    initStackedWidget();
    initTabSwitching();
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

void AdminDialog::initTabSwitching()
{
    QObject::connect(ui.usersBtn, SIGNAL(clicked()), this, SLOT(onTabBtnClicked()));
}

void AdminDialog::onTabBtnClicked()
{
    QWidget* widget = qobject_cast<QWidget*>(sender());
    ui.stackedWidget->setCurrentWidget(widget);
}
