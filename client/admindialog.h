#ifndef ADMINDIALOG_H
#define ADMINDIALOG_H

#include <QDialog>
#include "ui_admindialog.h"
#include "userswidget.h"
#include "allpricelistswidget.h"
#include "adstablewidget.h"

class AdminDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminDialog(QWidget *parent = nullptr);

private slots:
    //users button clicked in side menu, show usersWidget
    void on_usersBtn_clicked();
    //price lists button clicked in side menu, show pricesWidget
    void on_priceListsBtn_clicked();
    //ads table button clicked, show adsTableWidget
    void on_adsBtn_clicked();

private:
    Ui::AdminDialog ui;

    //widgets instances that will be shown after selecting tab in side menu
    //*****************

    //widget holding list of all users obtained from db
    UsersWidget usersWidget;

    //widget presenting all available price lists
    AllPriceListsWidget pricesWidget;

    //widget with all user' ads
    AdsTableWidget adsTableWidget;

    //*****************

    //adds widgets to stacket widget object
    void initStackedWidget();
};

#endif // ADMINDIALOG_H
