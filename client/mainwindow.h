#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ui_mainwindow.h>
#include "loginwidget.h"
#include "mainwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    Ui::MainWindow ui;

    //widget displaying login form
    LoginWidget loginWidget;
    //widget displaying main application panel
    MainWidget mainWidget;

    //action for toolbar's logout button
    QAction* logoutAction;

    //initialize stacked widget with login and main views
    void initStackedWidget();

    //makes main toolbar's items right aligned
    void initToolbarItemsRightAlignment();

    //adds logout button to main toolbar
    void addLogoutBtn();

private slots:
    //widget displayed in stacked widget was changed
    void onStackedWidgetChanged(int index);

    //user gave valid login credentials during login process
    void onUserLoggedIn();

    //user clicked logout buton
    void onLogout();
};

#endif // MAINWINDOW_H
