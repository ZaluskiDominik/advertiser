#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ui_mainwindow.h>
#include "loginwidget.h"
#include "mainwidget.h"
#include "profiledialog.h"
#include "mainmenu.h"
#include "admindialog.h"
#include "pricelistwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    Ui::MainWindow ui;
    MainMenu* mainMenu;

    //widget displaying login form
    LoginWidget loginWidget;
    //widget displaying main application panel
    MainWidget mainWidget;

    //dialog for editing user's data
    ProfileDialog* profileDialog;
    //admin's main panel
    AdminDialog* adminDialog;

    //action for toolbar's logout button
    QAction* logoutAction;
    //label with user's login
    QAction* loginLabelAction;
    QLabel* loginLabel;
    //label informing s user is an admin
    QAction* adminLabelAction;
    //action for opening menu for editing profile
    QAction* profileToolAction;

    //initialize stacked widget with login and main views
    void initStackedWidget();

    //create main menu, connects menu options signals to slots
    void initMenu();

    //TOOLBAR
    void initToolBarActions();
    //makes main toolbar's items right aligned
    void initToolbarItemsRightAlignment();
    //adds label with loged in user's login
    void addLoginLabel();
    //adds appropriate label if user is an admin
    void addAdminLabel();
    //adds label with profile image
    void addProfileLabel();
    //adds logout button to main toolbar
    void addLogoutBtn();

private slots:
    //connection with server was lost
    void onDisconnectFromServer();

    //widget displayed in stacked widget was changed
    void onStackedWidgetChanged(int index);

    //user gave valid login credentials during login process
    void onUserLoggedIn();

    //user's data was edited in profileDialog
    void onUserDataChanged(UserData data);

    //user clicked logout button
    void onLogout();

    //profile action was triggered, show profile data
    void onProfileClicked();

    //admin panel menu was clicked, show admin panel
    void onAdminPanelClicked();

    //price list menu was clicked, show price list
    void onPriceListClicked();
};

#endif // MAINWINDOW_H
