#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);
    initStackedWidget();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initStackedWidget()
{
    //add login form
    ui.stackedWidget->addWidget(&loginWidget);
    //add main panel

    //set default view to login form
    ui.stackedWidget->setCurrentWidget(&loginWidget);

    onStackedWidgetChanged( ui.stackedWidget->indexOf(&loginWidget) );
    QObject::connect(ui.stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(onStackedWidgetChanged(int)));
}

void MainWindow::onStackedWidgetChanged(int index)
{
    if ( ui.stackedWidget->indexOf(&loginWidget) == index )
    {
        //changed to login
        setFixedSize(700, 500);
    }
    else
    {
        //changed to main panel
        setMinimumSize(400, 500);
        setMaximumSize(9999, 9999);
    }
}
