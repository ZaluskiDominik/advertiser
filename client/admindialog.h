#ifndef ADMINDIALOG_H
#define ADMINDIALOG_H

#include <QDialog>
#include "ui_admindialog.h"
#include "userswidget.h"

class AdminDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminDialog(QWidget *parent = nullptr);
    ~AdminDialog();

private slots:
    //users button clicked in side menu, show usersWidget
    void on_usersBtn_clicked();

private:
    Ui::AdminDialog ui;

    //widgets instances that will be shown after selecting tab in side menu
    //*****************

    //widget holding list of all users obtained from db
    UsersWidget usersWidget;
    //*****************

    //adds widgets to stacket widget object
    void initStackedWidget();
};

#endif // ADMINDIALOG_H
