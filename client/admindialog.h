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

private:
    Ui::AdminDialog ui;

    //widgets instances that will be shown after selecting tab in side menu

    //widget holding list of all users obtained from db
    UsersWidget usersWidget;

    //adds widgets to stacket widget object
    void initStackedWidget();

    //connects menu's buttons' clicked signals slot that will be changing current widget in stacket widget
    void initTabSwitching();

private slots:
    //button in side menu was clicked, change current widget in stacked widget
    void onTabBtnClicked();
};

#endif // ADMINDIALOG_H
