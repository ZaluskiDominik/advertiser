#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "sockethandler.h"
#include "userdata.h"
#include <QGraphicsDropShadowEffect>

//global socketHandler to which all client request are registered
SocketHandler socketHandler;
//data of currently logged in user
UserData user;

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent)
{
    ui.setupUi(this);
    initMenu();
    initStackedWidget();
    initToolBarActions();
}

void MainWindow::initStackedWidget()
{
    //add login form
    ui.stackedWidget->addWidget(&loginWidget);
    QObject::connect(&loginWidget, SIGNAL(userLoggedIn()), this, SLOT(onUserLoggedIn()));
    //add main panel
    ui.stackedWidget->addWidget(&mainWidget);

    //set default view to login form
    ui.stackedWidget->setCurrentWidget(&loginWidget);

    //connect to onWidgetChanged signal
    onStackedWidgetChanged( ui.stackedWidget->indexOf(&loginWidget) );
    QObject::connect(ui.stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetChanged(int)));
}

void MainWindow::initMenu()
{
    mainMenu = new MainMenu(this);
    setMenuWidget(mainMenu);
    QObject::connect(mainMenu, SIGNAL(profileClicked()), this, SLOT(onProfileClicked()));
}

void MainWindow::initToolBarActions()
{
    initToolbarItemsRightAlignment();
    addAdminLabel();
    addLoginLabel();
    addProfileLabel();
    addLogoutBtn();
}

void MainWindow::initToolbarItemsRightAlignment()
{
    QWidget* spaceFiller = new QWidget(this);
    spaceFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui.mainToolBar->addWidget(spaceFiller);
}

void MainWindow::addLoginLabel()
{
    loginLabel = new QLabel;
    loginLabelAction = ui.mainToolBar->addWidget(loginLabel);
}

void MainWindow::addAdminLabel()
{
    QLabel* adminLabel = new QLabel("Admin");
    adminLabelAction = ui.mainToolBar->addWidget(adminLabel);
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(adminLabel);
    shadow->setOffset(2, 2);
    shadow->setBlurRadius(3);
    shadow->setColor(Qt::gray);
    adminLabel->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
    adminLabel->setGraphicsEffect(shadow);
}

void MainWindow::addProfileLabel()
{
    profileToolAction = new QAction(QIcon(":/img/img/profile.png"), "Profile");
    profileToolAction->setToolTip("Wyświetl swój profil");
    ui.mainToolBar->addAction(profileToolAction);

    QObject::connect(profileToolAction, SIGNAL(triggered()), this, SLOT(onProfileClicked()));
}

void MainWindow::addLogoutBtn()
{
    logoutAction = new QAction(QIcon(":/img/img/logout.png"), "Wyloguj");
    logoutAction->setToolTip("Wyloguj się");
    ui.mainToolBar->addAction(logoutAction);

    QObject::connect(logoutAction, SIGNAL(triggered()), this, SLOT(onLogout()));
}

void MainWindow::onStackedWidgetChanged(int index)
{
    if ( ui.stackedWidget->indexOf(&loginWidget) == index )
    {
        //changed to login, set fixed size of the screen
        setFixedSize(700, 500);
        resize(700, 500);
        //hide menu and tool bar
        ui.mainToolBar->hide();
        menuWidget()->hide();
    }
    else
    {
        //changed to main panel, unset fixed size of the screen
        setMinimumSize(400, 500);
        setMaximumSize(9999, 9999);
        //display menu and toolbar
        ui.mainToolBar->show();
        menuWidget()->show();
    }
}

void MainWindow::onUserLoggedIn()
{
    //change view to main widget
    ui.stackedWidget->setCurrentWidget(&mainWidget);

    //if user is an admin show label informing about that, else show user's login
    adminLabelAction->setVisible(user.isAdmin);
    loginLabelAction->setVisible(!user.isAdmin);
    if (!user.isAdmin)
        loginLabel->setText(user.login);
}

void MainWindow::onLogout()
{
    ui.stackedWidget->setCurrentWidget(&loginWidget);
}

void MainWindow::onProfileClicked()
{
    ProfileDialog* dialog = new ProfileDialog(user);
    dialog->show();
}
