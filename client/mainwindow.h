#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ui_mainwindow.h>
#include "loginwidget.h"
#include "mainwidget.h"
#include "profiledialog.h"
#include "mainmenu.h"

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
    //widget displayed in stacked widget was changed
    void onStackedWidgetChanged(int index);

    //user gave valid login credentials during login process
    void onUserLoggedIn();

    //user clicked logout buton
    void onLogout();

    //profile action was triggered
    void onProfileClicked();
};

#endif // MAINWINDOW_H
