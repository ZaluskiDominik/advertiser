#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "../shared/sockethandler.h"
#include "userdata.h"
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>

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

    //connect disconnection from server slot to socket
    QObject::connect(&socketHandler, SIGNAL(disconnected(SocketHandler*)), this, SLOT(onDisconnectFromServer()));
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

    //connect to menu options' signals
    QObject::connect(mainMenu, SIGNAL(profileClicked()), this, SLOT(onProfileClicked()));
    QObject::connect(mainMenu, SIGNAL(adminPanelClicked()), this, SLOT(onAdminPanelClicked()));
    QObject::connect(mainMenu, SIGNAL(priceListClicked()), this, SLOT(onPriceListClicked()));
    QObject::connect(mainMenu, SIGNAL(adsEditorClicked()), this, SLOT(onAdsClicked()));
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
    //create dummy widget that will take all available space on the left
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

    //connect to clicked signal
    QObject::connect(profileToolAction, SIGNAL(triggered()), this, SLOT(onProfileClicked()));
}

void MainWindow::addLogoutBtn()
{
    logoutAction = new QAction(QIcon(":/img/img/logout.png"), "Wyloguj");
    logoutAction->setToolTip("Wyloguj się");
    ui.mainToolBar->addAction(logoutAction);

    //connect to clicked signal
    QObject::connect(logoutAction, SIGNAL(triggered()), this, SLOT(onLogout()));
}

void MainWindow::onDisconnectFromServer()
{
    if ( ui.stackedWidget->currentWidget() != &loginWidget )
    {
        QMessageBox::warning(this, "Stracono połączenie z serwerem", "Utracono połączenie z serwerem. Zostaniesz wylogowany...");
        //change screen to login form view
        onLogout();
    }
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

    //if user is an admin show admin panel menu option
    mainMenu->showAdminPanelMenu(user.isAdmin);
    //if user is an admin disable regula users options
    mainMenu->disableNonAdminOptions(user.isAdmin);

    //if user isn't an admin, set loginLabel's text to user's login
    if (!user.isAdmin)
        loginLabel->setText(user.login);
}

void MainWindow::onUserDataChanged(UserData data)
{
    user = data;
}

void MainWindow::onLogout()
{
    ui.stackedWidget->setCurrentWidget(&loginWidget);
}

void MainWindow::onProfileClicked()
{
    profileDialog = new ProfileDialog(user, this);
    QObject::connect(profileDialog, SIGNAL(userDataChanged(UserData)), this, SLOT(onUserDataChanged(UserData)));
    profileDialog->show();
}

void MainWindow::onAdminPanelClicked()
{
    adminDialog = new AdminDialog(this);
    adminDialog->setAttribute(Qt::WA_DeleteOnClose);
    adminDialog->show();
}

void MainWindow::onPriceListClicked()
{
    //create dialog
    QDialog* priceDialog = new QDialog(this);
    priceDialog->setModal(true);
    priceDialog->setAttribute(Qt::WA_DeleteOnClose);

    //create layout for dialog
    QHBoxLayout* priceLayout = new QHBoxLayout(priceDialog);
    priceLayout->addWidget( new PriceListWidget(true) );
    priceLayout->setMargin(0);

    //resize dialog, apply layout and show everything on the screen
    priceDialog->setMinimumSize(700, 550);
    priceDialog->setLayout(priceLayout);
    priceDialog->show();
}

void MainWindow::onAdsClicked()
{
    adsDialog = new AdsDialog(this);
    adsDialog->setAttribute(Qt::WA_DeleteOnClose);
    adsDialog->show();
}
